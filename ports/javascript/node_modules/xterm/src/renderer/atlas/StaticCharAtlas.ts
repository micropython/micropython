/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { DIM_OPACITY, IGlyphIdentifier, ICharAtlasConfig, CHAR_ATLAS_CELL_SPACING } from './Types';
import { generateStaticCharAtlasTexture } from './CharAtlasGenerator';
import BaseCharAtlas from './BaseCharAtlas';
import { is256Color } from './CharAtlasUtils';
import { DEFAULT_COLOR } from '../../common/Types';

export default class StaticCharAtlas extends BaseCharAtlas {
  private _texture: HTMLCanvasElement | ImageBitmap;

  constructor(private _document: Document, private _config: ICharAtlasConfig) {
    super();
  }

  private _canvasFactory = (width: number, height: number) => {
    const canvas = this._document.createElement('canvas');
    canvas.width = width;
    canvas.height = height;

    // This is useful for debugging
    // document.body.appendChild(canvas);

    return canvas;
  }

  protected _doWarmUp(): void {
    const result = generateStaticCharAtlasTexture(window, this._canvasFactory, this._config);
    if (result instanceof HTMLCanvasElement) {
      this._texture = result;
    } else {
      result.then(texture => {
        this._texture = texture;
      });
    }
  }

  private _isCached(glyph: IGlyphIdentifier, colorIndex: number): boolean {
    const isAscii = glyph.code < 256;
    // A color is basic if it is one of the 4 bit ANSI colors.
    const isBasicColor = glyph.fg < 16;
    const isDefaultColor = glyph.fg === DEFAULT_COLOR;
    const isDefaultBackground = glyph.bg === DEFAULT_COLOR;
    return isAscii && (isBasicColor || isDefaultColor) && isDefaultBackground && !glyph.italic;
  }

  public draw(
    ctx: CanvasRenderingContext2D,
    glyph: IGlyphIdentifier,
    x: number,
    y: number
  ): boolean {
    // we're not warmed up yet
    if (this._texture === null || this._texture === undefined) {
      return false;
    }

    let colorIndex = 0;
    if (is256Color(glyph.fg)) {
      colorIndex = 2 + glyph.fg + (glyph.bold ? 16 : 0);
    } else if (glyph.fg === DEFAULT_COLOR) {
      // If default color and bold
      if (glyph.bold) {
        colorIndex = 1;
      }
    }
    if (!this._isCached(glyph, colorIndex)) {
      return false;
    }

    ctx.save();

    // ImageBitmap's draw about twice as fast as from a canvas
    const charAtlasCellWidth = this._config.scaledCharWidth + CHAR_ATLAS_CELL_SPACING;
    const charAtlasCellHeight = this._config.scaledCharHeight + CHAR_ATLAS_CELL_SPACING;

    // Apply alpha to dim the character
    if (glyph.dim) {
      ctx.globalAlpha = DIM_OPACITY;
    }

    ctx.drawImage(
      this._texture,
      glyph.code * charAtlasCellWidth,
      colorIndex * charAtlasCellHeight,
      charAtlasCellWidth,
      this._config.scaledCharHeight,
      x,
      y,
      charAtlasCellWidth,
      this._config.scaledCharHeight
    );

    ctx.restore();

    return true;
  }
}
