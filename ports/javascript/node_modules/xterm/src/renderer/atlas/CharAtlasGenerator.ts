/**
 * Copyright (c) 2018 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { FontWeight } from 'xterm';
import { isFirefox, isSafari } from '../../common/Platform';
import { ICharAtlasConfig, CHAR_ATLAS_CELL_SPACING } from './Types';
import { IColor } from '../../ui/Types';

/**
 * Generates a char atlas.
 * @param context The window or worker context.
 * @param canvasFactory A function to generate a canvas with a width or height.
 * @param config The config for the new char atlas.
 */
export function generateStaticCharAtlasTexture(context: Window, canvasFactory: (width: number, height: number) => HTMLCanvasElement, config: ICharAtlasConfig): HTMLCanvasElement | Promise<ImageBitmap> {
  const cellWidth = config.scaledCharWidth + CHAR_ATLAS_CELL_SPACING;
  const cellHeight = config.scaledCharHeight + CHAR_ATLAS_CELL_SPACING;
  const canvas = canvasFactory(
    /*255 ascii chars*/255 * cellWidth,
    (/*default+default bold*/2 + /*0-15*/16 + /*0-15 bold*/16) * cellHeight
  );
  const ctx = canvas.getContext('2d', {alpha: config.allowTransparency});

  ctx.fillStyle = config.colors.background.css;
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  ctx.save();
  ctx.fillStyle = config.colors.foreground.css;
  ctx.font = getFont(config.fontWeight, config);
  ctx.textBaseline = 'middle';

  // Default color
  for (let i = 0; i < 256; i++) {
    ctx.save();
    ctx.beginPath();
    ctx.rect(i * cellWidth, 0, cellWidth, cellHeight);
    ctx.clip();
    ctx.fillText(String.fromCharCode(i), i * cellWidth, cellHeight / 2);
    ctx.restore();
  }
  // Default color bold
  ctx.save();
  ctx.font = getFont(config.fontWeightBold, config);
  for (let i = 0; i < 256; i++) {
    ctx.save();
    ctx.beginPath();
    ctx.rect(i * cellWidth, cellHeight, cellWidth, cellHeight);
    ctx.clip();
    ctx.fillText(String.fromCharCode(i), i * cellWidth, cellHeight * 1.5);
    ctx.restore();
  }
  ctx.restore();

  // Colors 0-15
  ctx.font = getFont(config.fontWeight, config);
  for (let colorIndex = 0; colorIndex < 16; colorIndex++) {
    const y = (colorIndex + 2) * cellHeight;
    // Draw ascii characters
    for (let i = 0; i < 256; i++) {
      ctx.save();
      ctx.beginPath();
      ctx.rect(i * cellWidth, y, cellWidth, cellHeight);
      ctx.clip();
      ctx.fillStyle = config.colors.ansi[colorIndex].css;
      ctx.fillText(String.fromCharCode(i), i * cellWidth, y + cellHeight / 2);
      ctx.restore();
    }
  }

  // Colors 0-15 bold
  ctx.font = getFont(config.fontWeightBold, config);
  for (let colorIndex = 0; colorIndex < 16; colorIndex++) {
    const y = (colorIndex + 2 + 16) * cellHeight;
    // Draw ascii characters
    for (let i = 0; i < 256; i++) {
      ctx.save();
      ctx.beginPath();
      ctx.rect(i * cellWidth, y, cellWidth, cellHeight);
      ctx.clip();
      ctx.fillStyle = config.colors.ansi[colorIndex].css;
      ctx.fillText(String.fromCharCode(i), i * cellWidth, y + cellHeight / 2);
      ctx.restore();
    }
  }
  ctx.restore();

  // Support is patchy for createImageBitmap at the moment, pass a canvas back
  // if support is lacking as drawImage works there too. Firefox is also
  // included here as ImageBitmap appears both buggy and has horrible
  // performance (tested on v55).
  if (!('createImageBitmap' in context) || isFirefox || isSafari) {
    // Don't attempt to clear background colors if createImageBitmap is not supported
    return canvas;
  }

  const charAtlasImageData = ctx.getImageData(0, 0, canvas.width, canvas.height);

  // Remove the background color from the image so characters may overlap
  clearColor(charAtlasImageData, config.colors.background);

  return context.createImageBitmap(charAtlasImageData);
}

/**
 * Makes a partiicular rgb color in an ImageData completely transparent.
 * @returns True if the result is "empty", meaning all pixels are fully transparent.
 */
export function clearColor(imageData: ImageData, color: IColor): boolean {
  let isEmpty = true;
  const r = color.rgba >>> 24;
  const g = color.rgba >>> 16 & 0xFF;
  const b = color.rgba >>> 8 & 0xFF;
  for (let offset = 0; offset < imageData.data.length; offset += 4) {
    if (imageData.data[offset] === r &&
        imageData.data[offset + 1] === g &&
        imageData.data[offset + 2] === b) {
      imageData.data[offset + 3] = 0;
    } else {
      isEmpty = false;
    }
  }
  return isEmpty;
}

function getFont(fontWeight: FontWeight, config: ICharAtlasConfig): string {
  return `${fontWeight} ${config.fontSize * config.devicePixelRatio}px ${config.fontFamily}`;
}
