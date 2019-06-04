/**
 * Copyright (c) 2018 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { EventEmitter2, IEvent } from '../../common/EventEmitter2';
import { Disposable } from '../../common/Lifecycle';
import { IMarker } from '../Types';

export class Marker extends Disposable implements IMarker {
  private static _nextId = 1;

  private _id: number = Marker._nextId++;
  public isDisposed: boolean = false;

  public get id(): number { return this._id; }

  private _onDispose = new EventEmitter2<void>();
  public get onDispose(): IEvent<void> { return this._onDispose.event; }

  constructor(
    public line: number
  ) {
    super();
  }

  public dispose(): void {
    if (this.isDisposed) {
      return;
    }
    this.isDisposed = true;
    // Emit before super.dispose such that dispose listeners get a change to react
    this._onDispose.fire();
  }
}
