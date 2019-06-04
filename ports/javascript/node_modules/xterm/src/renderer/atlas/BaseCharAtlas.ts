/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { IGlyphIdentifier } from './Types';
import { IDisposable } from 'xterm';

export default abstract class BaseCharAtlas implements IDisposable {
  private _didWarmUp: boolean = false;

  public dispose(): void { }

  /**
   * Perform any work needed to warm the cache before it can be used. May be called multiple times.
   * Implement _doWarmUp instead if you only want to get called once.
   */
  public warmUp(): void {
    if (!this._didWarmUp) {
      this._doWarmUp();
      this._didWarmUp = true;
    }
  }

  /**
   * Perform any work needed to warm the cache before it can be used. Used by the default
   * implementation of warmUp(), and will only be called once.
   */
  protected _doWarmUp(): void { }

  /**
   * Called when we start drawing a new frame.
   *
   * TODO: We rely on this getting called by TextRenderLayer. This should really be called by
   * Renderer instead, but we need to make Renderer the source-of-truth for the char atlas, instead
   * of BaseRenderLayer.
   */
  public beginFrame(): void { }

  /**
   * May be called before warmUp finishes, however it is okay for the implementation to
   * do nothing and return false in that case.
   *
   * @param ctx Where to draw the character onto.
   * @param glyph Information about what to draw
   * @param x The position on the context to start drawing at
   * @param y The position on the context to start drawing at
   * @returns The success state. True if we drew the character.
   */
  public abstract draw(
    ctx: CanvasRenderingContext2D,
    glyph: IGlyphIdentifier,
    x: number,
    y: number
  ): boolean;
}
