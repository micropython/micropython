/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { FontWeight } from 'xterm';
import { IColorSet } from '../../ui/Types';

export const INVERTED_DEFAULT_COLOR = 257;
export const DIM_OPACITY = 0.5;

export const CHAR_ATLAS_CELL_SPACING = 1;

export interface IGlyphIdentifier {
  chars: string;
  code: number;
  bg: number;
  fg: number;
  bold: boolean;
  dim: boolean;
  italic: boolean;
}

export interface ICharAtlasConfig {
  type: 'none' | 'static' | 'dynamic';
  devicePixelRatio: number;
  fontSize: number;
  fontFamily: string;
  fontWeight: FontWeight;
  fontWeightBold: FontWeight;
  scaledCharWidth: number;
  scaledCharHeight: number;
  allowTransparency: boolean;
  colors: IColorSet;
}
