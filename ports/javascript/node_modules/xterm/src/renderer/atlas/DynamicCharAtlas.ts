/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { DIM_OPACITY, IGlyphIdentifier, INVERTED_DEFAULT_COLOR, ICharAtlasConfig } from './Types';
import BaseCharAtlas from './BaseCharAtlas';
import { DEFAULT_ANSI_COLORS } from '../../ui/ColorManager';
import { clearColor } from './CharAtlasGenerator';
import LRUMap from './LRUMap';
import { isFirefox, isSafari } from '../../common/Platform';
import { IColor } from '../../ui/Types';

// In practice we're probably never going to exhaust a texture this large. For debugging purposes,
// however, it can be useful to set this to a really tiny value, to verify that LRU eviction works.
const TEXTURE_WIDTH = 1024;
const TEXTURE_HEIGHT = 1024;

const TRANSPARENT_COLOR = {
  css: 'rgba(0, 0, 0, 0)',
  rgba: 0
};

// Drawing to the cache is expensive: If we have to draw more than this number of glyphs to the
// cache in a single frame, give up on trying to cache anything else, and try to finish the current
// frame ASAP.
//
// This helps to limit the amount of damage a program can do when it would otherwise thrash the
// cache.
const FRAME_CACHE_DRAW_LIMIT = 100;

/**
 * The number of milliseconds to wait before generating the ImageBitmap, this is to debounce/batch
 * the operation as window.createImageBitmap is asynchronous.
 */
const GLYPH_BITMAP_COMMIT_DELAY = 100;

interface IGlyphCacheValue {
  index: number;
  isEmpty: boolean;
  inBitmap: boolean;
}

export function getGlyphCacheKey(glyph: IGlyphIdentifier): number {
  // Note that this only returns a valid key when code < 256
  // Layout:
  // 0b00000000000000000000000000000001: italic (1)
  // 0b00000000000000000000000000000010: dim (1)
  // 0b00000000000000000000000000000100: bold (1)
  // 0b00000000000000000000111111111000: fg (9)
  // 0b00000000000111111111000000000000: bg (9)
  // 0b00011111111000000000000000000000: code (8)
  // 0b11100000000000000000000000000000: unused (3)
  return glyph.code << 21 | glyph.bg << 12 | glyph.fg << 3 | (glyph.bold ? 0 : 4) + (glyph.dim ? 0 : 2) + (glyph.italic ? 0 : 1);
}

export default class DynamicCharAtlas extends BaseCharAtlas {
  // An ordered map that we're using to keep track of where each glyph is in the atlas texture.
  // It's ordered so that we can determine when to remove the old entries.
  private _cacheMap: LRUMap<IGlyphCacheValue>;

  // The texture that the atlas is drawn to
  private _cacheCanvas: HTMLCanvasElement;
  private _cacheCtx: CanvasRenderingContext2D;

  // A temporary context that glyphs are drawn to before being transfered to the atlas.
  private _tmpCtx: CanvasRenderingContext2D;

  // The number of characters stored in the atlas by width/height
  private _width: number;
  private _height: number;

  private _drawToCacheCount: number = 0;

  // An array of glyph keys that are waiting on the bitmap to be generated.
  private _glyphsWaitingOnBitmap: IGlyphCacheValue[] = [];

  // The timeout that is used to batch bitmap generation so it's not requested for every new glyph.
  private _bitmapCommitTimeout: number | null = null;

  // The bitmap to draw from, this is much faster on other browsers than others.
  private _bitmap: ImageBitmap | null = null;

  constructor(document: Document, private _config: ICharAtlasConfig) {
    super();
    this._cacheCanvas = document.createElement('canvas');
    this._cacheCanvas.width = TEXTURE_WIDTH;
    this._cacheCanvas.height = TEXTURE_HEIGHT;
    // The canvas needs alpha because we use clearColor to convert the background color to alpha.
    // It might also contain some characters with transparent backgrounds if allowTransparency is
    // set.
    this._cacheCtx = this._cacheCanvas.getContext('2d', {alpha: true});

    const tmpCanvas = document.createElement('canvas');
    tmpCanvas.width = this._config.scaledCharWidth;
    tmpCanvas.height = this._config.scaledCharHeight;
    this._tmpCtx = tmpCanvas.getContext('2d', {alpha: this._config.allowTransparency});

    this._width = Math.floor(TEXTURE_WIDTH / this._config.scaledCharWidth);
    this._height = Math.floor(TEXTURE_HEIGHT / this._config.scaledCharHeight);
    const capacity = this._width * this._height;
    this._cacheMap = new LRUMap(capacity);
    this._cacheMap.prealloc(capacity);

    // This is useful for debugging
    // document.body.appendChild(this._cacheCanvas);
  }

  public dispose(): void {
    if (this._bitmapCommitTimeout !== null) {
      window.clearTimeout(this._bitmapCommitTimeout);
      this._bitmapCommitTimeout = null;
    }
  }

  public beginFrame(): void {
    this._drawToCacheCount = 0;
  }

  public draw(
    ctx: CanvasRenderingContext2D,
    glyph: IGlyphIdentifier,
    x: number,
    y: number
  ): boolean {
    // Space is always an empty cell, special case this as it's so common
    if (glyph.code === 32) {
      return true;
    }

    // Exit early for uncachable glyphs
    if (!this._canCache(glyph)) {
      return false;
    }

    const glyphKey = getGlyphCacheKey(glyph);
    const cacheValue = this._cacheMap.get(glyphKey);
    if (cacheValue !== null && cacheValue !== undefined) {
      this._drawFromCache(ctx, cacheValue, x, y);
      return true;
    } else if (this._drawToCacheCount < FRAME_CACHE_DRAW_LIMIT) {
      let index;
      if (this._cacheMap.size < this._cacheMap.capacity) {
        index = this._cacheMap.size;
      } else {
        // we're out of space, so our call to set will delete this item
        index = this._cacheMap.peek().index;
      }
      const cacheValue = this._drawToCache(glyph, index);
      this._cacheMap.set(glyphKey, cacheValue);
      this._drawFromCache(ctx, cacheValue, x, y);
      return true;
    }
    return false;
  }

  private _canCache(glyph: IGlyphIdentifier): boolean {
    // Only cache ascii and extended characters for now, to be safe. In the future, we could do
    // something more complicated to determine the expected width of a character.
    //
    // If we switch the renderer over to webgl at some point, we may be able to use blending modes
    // to draw overlapping glyphs from the atlas:
    // https://github.com/servo/webrender/issues/464#issuecomment-255632875
    // https://webglfundamentals.org/webgl/lessons/webgl-text-texture.html
    return glyph.code < 256;
  }

  private _toCoordinateX(index: number): number {
    return (index % this._width) * this._config.scaledCharWidth;
  }

  private _toCoordinateY(index: number): number {
    return Math.floor(index / this._width) * this._config.scaledCharHeight;
  }

  private _drawFromCache(
    ctx: CanvasRenderingContext2D,
    cacheValue: IGlyphCacheValue,
    x: number,
    y: number
  ): void {
    // We don't actually need to do anything if this is whitespace.
    if (cacheValue.isEmpty) {
      return;
    }
    const cacheX = this._toCoordinateX(cacheValue.index);
    const cacheY = this._toCoordinateY(cacheValue.index);
    ctx.drawImage(
      cacheValue.inBitmap ? this._bitmap : this._cacheCanvas,
      cacheX,
      cacheY,
      this._config.scaledCharWidth,
      this._config.scaledCharHeight,
      x,
      y,
      this._config.scaledCharWidth,
      this._config.scaledCharHeight
    );
  }

  private _getColorFromAnsiIndex(idx: number): IColor {
    if (idx < this._config.colors.ansi.length) {
      return this._config.colors.ansi[idx];
    }
    return DEFAULT_ANSI_COLORS[idx];
  }

  private _getBackgroundColor(glyph: IGlyphIdentifier): IColor {
    if (this._config.allowTransparency) {
      // The background color might have some transparency, so we need to render it as fully
      // transparent in the atlas. Otherwise we'd end up drawing the transparent background twice
      // around the anti-aliased edges of the glyph, and it would look too dark.
      return TRANSPARENT_COLOR;
    } else if (glyph.bg === INVERTED_DEFAULT_COLOR) {
      return this._config.colors.foreground;
    } else if (glyph.bg < 256) {
      return this._getColorFromAnsiIndex(glyph.bg);
    }
    return this._config.colors.background;
  }

  private _getForegroundColor(glyph: IGlyphIdentifier): IColor {
    if (glyph.fg === INVERTED_DEFAULT_COLOR) {
      return this._config.colors.background;
    } else if (glyph.fg < 256) {
      // 256 color support
      return this._getColorFromAnsiIndex(glyph.fg);
    }
    return this._config.colors.foreground;
  }

  // TODO: We do this (or something similar) in multiple places. We should split this off
  // into a shared function.
  private _drawToCache(glyph: IGlyphIdentifier, index: number): IGlyphCacheValue {
    this._drawToCacheCount++;

    this._tmpCtx.save();

    // draw the background
    const backgroundColor = this._getBackgroundColor(glyph);
    // Use a 'copy' composite operation to clear any existing glyph out of _tmpCtxWithAlpha, regardless of
    // transparency in backgroundColor
    this._tmpCtx.globalCompositeOperation = 'copy';
    this._tmpCtx.fillStyle = backgroundColor.css;
    this._tmpCtx.fillRect(0, 0, this._config.scaledCharWidth, this._config.scaledCharHeight);
    this._tmpCtx.globalCompositeOperation = 'source-over';

    // draw the foreground/glyph
    const fontWeight = glyph.bold ? this._config.fontWeightBold : this._config.fontWeight;
    const fontStyle = glyph.italic ? 'italic' : '';
    this._tmpCtx.font =
      `${fontStyle} ${fontWeight} ${this._config.fontSize * this._config.devicePixelRatio}px ${this._config.fontFamily}`;
    this._tmpCtx.textBaseline = 'middle';

    this._tmpCtx.fillStyle = this._getForegroundColor(glyph).css;

    // Apply alpha to dim the character
    if (glyph.dim) {
      this._tmpCtx.globalAlpha = DIM_OPACITY;
    }
    // Draw the character
    this._tmpCtx.fillText(glyph.chars, 0, this._config.scaledCharHeight / 2);
    this._tmpCtx.restore();

    // clear the background from the character to avoid issues with drawing over the previous
    // character if it extends past it's bounds
    const imageData = this._tmpCtx.getImageData(
      0, 0, this._config.scaledCharWidth, this._config.scaledCharHeight
    );
    let isEmpty = false;
    if (!this._config.allowTransparency) {
      isEmpty = clearColor(imageData, backgroundColor);
    }

    // copy the data from imageData to _cacheCanvas
    const x = this._toCoordinateX(index);
    const y = this._toCoordinateY(index);
    // putImageData doesn't do any blending, so it will overwrite any existing cache entry for us
    this._cacheCtx.putImageData(imageData, x, y);

    // Add the glyph and queue it to the bitmap (if the browser supports it)
    const cacheValue = {
      index,
      isEmpty,
      inBitmap: false
    };
    this._addGlyphToBitmap(cacheValue);

    return cacheValue;
  }

  private _addGlyphToBitmap(cacheValue: IGlyphCacheValue): void {
    // Support is patchy for createImageBitmap at the moment, pass a canvas back
    // if support is lacking as drawImage works there too. Firefox is also
    // included here as ImageBitmap appears both buggy and has horrible
    // performance (tested on v55).
    if (!('createImageBitmap' in window) || isFirefox || isSafari) {
      return;
    }

    // Add the glyph to the queue
    this._glyphsWaitingOnBitmap.push(cacheValue);

    // Check if bitmap generation timeout already exists
    if (this._bitmapCommitTimeout !== null) {
      return;
    }

    this._bitmapCommitTimeout = window.setTimeout(() => this._generateBitmap(), GLYPH_BITMAP_COMMIT_DELAY);
  }

  private _generateBitmap(): void {
    const glyphsMovingToBitmap = this._glyphsWaitingOnBitmap;
    this._glyphsWaitingOnBitmap = [];
    window.createImageBitmap(this._cacheCanvas).then(bitmap => {
      // Set bitmap
      this._bitmap = bitmap;

      // Mark all new glyphs as in bitmap, excluding glyphs that came in after
      // the bitmap was requested
      for (let i = 0; i < glyphsMovingToBitmap.length; i++) {
        const value = glyphsMovingToBitmap[i];
        // It doesn't matter if the value was already evicted, it will be
        // released from memory after this block if so.
        value.inBitmap = true;
      }
    });
    this._bitmapCommitTimeout = null;
  }
}
