/**
 * Copyright (c) 2014 The xterm.js authors. All rights reserved.
 * @license MIT
 *
 * Fit terminal columns and rows to the dimensions of its DOM element.
 *
 * ## Approach
 *
 *    Rows: Truncate the division of the terminal parent element height by the
 *          terminal row height.
 * Columns: Truncate the division of the terminal parent element width by the
 *          terminal character width (apply display: inline at the terminal
 *          row and truncate its width with the current number of columns).
 */

import { Terminal } from 'xterm';

export interface IGeometry {
  rows: number;
  cols: number;
}

export function proposeGeometry(term: Terminal): IGeometry {
  if (!term.element.parentElement) {
    return null;
  }
  const parentElementStyle = window.getComputedStyle(term.element.parentElement);
  const parentElementHeight = parseInt(parentElementStyle.getPropertyValue('height'));
  const parentElementWidth = Math.max(0, parseInt(parentElementStyle.getPropertyValue('width')));
  const elementStyle = window.getComputedStyle(term.element);
  const elementPadding = {
    top: parseInt(elementStyle.getPropertyValue('padding-top')),
    bottom: parseInt(elementStyle.getPropertyValue('padding-bottom')),
    right: parseInt(elementStyle.getPropertyValue('padding-right')),
    left: parseInt(elementStyle.getPropertyValue('padding-left'))
  };
  const elementPaddingVer = elementPadding.top + elementPadding.bottom;
  const elementPaddingHor = elementPadding.right + elementPadding.left;
  const availableHeight = parentElementHeight - elementPaddingVer;
  const availableWidth = parentElementWidth - elementPaddingHor - (<any>term)._core.viewport.scrollBarWidth;
  const geometry = {
    cols: Math.floor(availableWidth / (<any>term)._core._renderCoordinator.dimensions.actualCellWidth),
    rows: Math.floor(availableHeight / (<any>term)._core._renderCoordinator.dimensions.actualCellHeight)
  };
  return geometry;
}

export function fit(term: Terminal): void {
  const geometry = proposeGeometry(term);
  if (geometry) {
    // Force a full render
    if (term.rows !== geometry.rows || term.cols !== geometry.cols) {
      (<any>term)._core._renderCoordinator.clear();
      term.resize(geometry.cols, geometry.rows);
    }
  }
}

export function apply(terminalConstructor: typeof Terminal): void {
  (<any>terminalConstructor.prototype).proposeGeometry = function (): IGeometry {
    return proposeGeometry(this);
  };

  (<any>terminalConstructor.prototype).fit = function (): void {
    fit(this);
  };
}
