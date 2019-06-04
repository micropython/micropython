/**
 * Copyright (c) 2018 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { IDisposable } from './Types';

/**
 * A base class that can be extended to provide convenience methods for managing the lifecycle of an
 * object and its components.
 */
export abstract class Disposable implements IDisposable {
  protected _disposables: IDisposable[] = [];
  protected _isDisposed: boolean = false;

  constructor() {
  }

  /**
   * Disposes the object, triggering the `dispose` method on all registered IDisposables.
   */
  public dispose(): void {
    this._isDisposed = true;
    this._disposables.forEach(d => d.dispose());
    this._disposables.length = 0;
  }

  /**
   * Registers a disposable object.
   * @param d The disposable to register.
   */
  public register<T extends IDisposable>(d: T): void {
    this._disposables.push(d);
  }

  /**
   * Unregisters a disposable object if it has been registered, if not do
   * nothing.
   * @param d The disposable to unregister.
   */
  public unregister<T extends IDisposable>(d: T): void {
    const index = this._disposables.indexOf(d);
    if (index !== -1) {
      this._disposables.splice(index, 1);
    }
  }
}
