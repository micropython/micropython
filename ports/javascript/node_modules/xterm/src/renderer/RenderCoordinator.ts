/**
 * Copyright (c) 2019 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { IRenderer, IRenderDimensions } from './Types';
import { RenderDebouncer } from '../ui/RenderDebouncer';
import { EventEmitter2, IEvent } from '../common/EventEmitter2';
import { Disposable } from '../common/Lifecycle';
import { ScreenDprMonitor } from '../ui/ScreenDprMonitor';
import { addDisposableDomListener } from '../ui/Lifecycle';
import { IColorSet } from '..//ui/Types';
import { CharacterJoinerHandler } from '../Types';

export class RenderCoordinator extends Disposable {
  private _renderDebouncer: RenderDebouncer;
  private _screenDprMonitor: ScreenDprMonitor;

  private _isPaused: boolean = false;
  private _needsFullRefresh: boolean = false;
  private _canvasWidth: number = 0;
  private _canvasHeight: number = 0;

  private _onDimensionsChange = new EventEmitter2<IRenderDimensions>();
  public get onDimensionsChange(): IEvent<IRenderDimensions> { return this._onDimensionsChange.event; }
  private _onRender = new EventEmitter2<{ start: number, end: number }>();
  public get onRender(): IEvent<{ start: number, end: number }> { return this._onRender.event; }
  private _onRefreshRequest = new EventEmitter2<{ start: number, end: number }>();
  public get onRefreshRequest(): IEvent<{ start: number, end: number }> { return this._onRefreshRequest.event; }

  public get dimensions(): IRenderDimensions { return this._renderer.dimensions; }

  constructor(
    private _renderer: IRenderer,
    private _rowCount: number,
    screenElement: HTMLElement
  ) {
    super();
    this._renderDebouncer = new RenderDebouncer((start, end) => this._renderRows(start, end));
    this.register(this._renderDebouncer);

    this._screenDprMonitor = new ScreenDprMonitor();
    this._screenDprMonitor.setListener(() => this._renderer.onDevicePixelRatioChange());
    this.register(this._screenDprMonitor);

    // dprchange should handle this case, we need this as well for browsers that don't support the
    // matchMedia query.
    this.register(addDisposableDomListener(window, 'resize', () => this._renderer.onDevicePixelRatioChange()));

    // Detect whether IntersectionObserver is detected and enable renderer pause
    // and resume based on terminal visibility if so
    if ('IntersectionObserver' in window) {
      const observer = new IntersectionObserver(e => this._onIntersectionChange(e[e.length - 1]), { threshold: 0 });
      observer.observe(screenElement);
      this.register({ dispose: () => observer.disconnect() });
    }
  }

  private _onIntersectionChange(entry: IntersectionObserverEntry): void {
    this._isPaused = entry.intersectionRatio === 0;
    if (!this._isPaused && this._needsFullRefresh) {
      this.refreshRows(0, this._rowCount - 1);
      this._needsFullRefresh = false;
    }
  }

  public refreshRows(start: number, end: number): void {
    if (this._isPaused) {
      this._needsFullRefresh = true;
      return;
    }
    this._renderDebouncer.refresh(start, end, this._rowCount);
  }

  private _renderRows(start: number, end: number): void {
    this._renderer.renderRows(start, end);
    this._onRender.fire({ start, end });
  }

  public resize(cols: number, rows: number): void {
    this._rowCount = rows;
    this._fireOnCanvasResize();
  }

  public changeOptions(): void {
    this._renderer.onOptionsChanged();
    this._fireOnCanvasResize();
  }

  private _fireOnCanvasResize(): void {
    // Don't fire the event if the dimensions haven't changed
    if (this._renderer.dimensions.canvasWidth === this._canvasWidth && this._renderer.dimensions.canvasHeight === this._canvasHeight) {
      return;
    }
    this._onDimensionsChange.fire(this._renderer.dimensions);
  }

  public setRenderer(renderer: IRenderer): void {
    // TODO: RenderCoordinator should be the only one to dispose the renderer
    this._renderer.dispose();
    this._renderer = renderer;
  }

  private _fullRefresh(): void {
    if (this._isPaused) {
      this._needsFullRefresh = true;
    } else {
      this.refreshRows(0, this._rowCount);
    }
  }

  public setColors(colors: IColorSet): void {
    this._renderer.setColors(colors);
    this._fullRefresh();
  }

  public onDevicePixelRatioChange(): void {
    this._renderer.onDevicePixelRatioChange();
  }

  public onResize(cols: number, rows: number): void {
    this._renderer.onResize(cols, rows);
    this._fullRefresh();
  }

  // TODO: Is this useful when we have onResize?
  public onCharSizeChanged(): void {
    this._renderer.onCharSizeChanged();
  }

  public onBlur(): void {
    this._renderer.onBlur();
  }

  public onFocus(): void {
    this._renderer.onFocus();
  }

  public onSelectionChanged(start: [number, number], end: [number, number], columnSelectMode: boolean): void {
    this._renderer.onSelectionChanged(start, end, columnSelectMode);
  }

  public onCursorMove(): void {
    this._renderer.onCursorMove();
  }

  public onOptionsChanged(): void {
    this._renderer.onOptionsChanged();
  }

  public clear(): void {
    this._renderer.clear();
  }

  public registerCharacterJoiner(handler: CharacterJoinerHandler): number {
    return this._renderer.registerCharacterJoiner(handler);
  }

  public deregisterCharacterJoiner(joinerId: number): boolean {
    return this._renderer.deregisterCharacterJoiner(joinerId);
  }
}
