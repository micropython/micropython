/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { ILinkifierEvent, ILinkMatcher, LinkMatcherHandler, ILinkMatcherOptions, ILinkifier, ITerminal, IBufferStringIteratorResult, IMouseZoneManager } from './Types';
import { MouseZone } from './MouseZoneManager';
import { getStringCellWidth } from './CharWidth';
import { EventEmitter2, IEvent } from './common/EventEmitter2';

/**
 * The Linkifier applies links to rows shortly after they have been refreshed.
 */
export class Linkifier implements ILinkifier {
  /**
   * The time to wait after a row is changed before it is linkified. This prevents
   * the costly operation of searching every row multiple times, potentially a
   * huge amount of times.
   */
  protected static readonly TIME_BEFORE_LINKIFY = 200;

  /**
   * Limit of the unwrapping line expansion (overscan) at the top and bottom
   * of the actual viewport in ASCII characters.
   * A limit of 2000 should match most sane urls.
   */
  protected static readonly OVERSCAN_CHAR_LIMIT = 2000;

  protected _linkMatchers: ILinkMatcher[] = [];

  private _mouseZoneManager: IMouseZoneManager;
  private _rowsTimeoutId: number;
  private _nextLinkMatcherId = 0;
  private _rowsToLinkify: { start: number, end: number };

  private _onLinkHover = new EventEmitter2<ILinkifierEvent>();
  public get onLinkHover(): IEvent<ILinkifierEvent> { return this._onLinkHover.event; }
  private _onLinkLeave = new EventEmitter2<ILinkifierEvent>();
  public get onLinkLeave(): IEvent<ILinkifierEvent> { return this._onLinkLeave.event; }
  private _onLinkTooltip = new EventEmitter2<ILinkifierEvent>();
  public get onLinkTooltip(): IEvent<ILinkifierEvent> { return this._onLinkTooltip.event; }

  constructor(
    protected _terminal: ITerminal
  ) {
    this._rowsToLinkify = {
      start: null,
      end: null
    };
  }

  /**
   * Attaches the linkifier to the DOM, enabling linkification.
   * @param mouseZoneManager The mouse zone manager to register link zones with.
   */
  public attachToDom(mouseZoneManager: IMouseZoneManager): void {
    this._mouseZoneManager = mouseZoneManager;
  }

  /**
   * Queue linkification on a set of rows.
   * @param start The row to linkify from (inclusive).
   * @param end The row to linkify to (inclusive).
   */
  public linkifyRows(start: number, end: number): void {
    // Don't attempt linkify if not yet attached to DOM
    if (!this._mouseZoneManager) {
      return;
    }

    // Increase range to linkify
    if (this._rowsToLinkify.start === null) {
      this._rowsToLinkify.start = start;
      this._rowsToLinkify.end = end;
    } else {
      this._rowsToLinkify.start = Math.min(this._rowsToLinkify.start, start);
      this._rowsToLinkify.end = Math.max(this._rowsToLinkify.end, end);
    }

    // Clear out any existing links on this row range
    this._mouseZoneManager.clearAll(start, end);

    // Restart timer
    if (this._rowsTimeoutId) {
      clearTimeout(this._rowsTimeoutId);
    }
    this._rowsTimeoutId = <number><any>setTimeout(() => this._linkifyRows(), Linkifier.TIME_BEFORE_LINKIFY);
  }

  /**
   * Linkifies the rows requested.
   */
  private _linkifyRows(): void {
    this._rowsTimeoutId = null;
    const buffer = this._terminal.buffer;

    // Ensure the start row exists
    const absoluteRowIndexStart = buffer.ydisp + this._rowsToLinkify.start;
    if (absoluteRowIndexStart >= buffer.lines.length) {
      return;
    }

    // Invalidate bad end row values (if a resize happened)
    const absoluteRowIndexEnd = buffer.ydisp + Math.min(this._rowsToLinkify.end, this._terminal.rows) + 1;

    // Iterate over the range of unwrapped content strings within start..end
    // (excluding).
    // _doLinkifyRow gets full unwrapped lines with the start row as buffer offset
    // for every matcher.
    // The unwrapping is needed to also match content that got wrapped across
    // several buffer lines. To avoid a worst case scenario where the whole buffer
    // contains just a single unwrapped string we limit this line expansion beyond
    // the viewport to +OVERSCAN_CHAR_LIMIT chars (overscan) at top and bottom.
    // This comes with the tradeoff that matches longer than OVERSCAN_CHAR_LIMIT
    // chars will not match anymore at the viewport borders.
    const overscanLineLimit = Math.ceil(Linkifier.OVERSCAN_CHAR_LIMIT / this._terminal.cols);
    const iterator = this._terminal.buffer.iterator(
      false, absoluteRowIndexStart, absoluteRowIndexEnd, overscanLineLimit, overscanLineLimit);
    while (iterator.hasNext()) {
      const lineData: IBufferStringIteratorResult = iterator.next();
      for (let i = 0; i < this._linkMatchers.length; i++) {
        this._doLinkifyRow(lineData.range.first, lineData.content, this._linkMatchers[i]);
      }
    }

    this._rowsToLinkify.start = null;
    this._rowsToLinkify.end = null;
  }

  /**
   * Registers a link matcher, allowing custom link patterns to be matched and
   * handled.
   * @param regex The regular expression to search for. Specifically, this
   * searches the textContent of the rows. You will want to use \s to match a
   * space ' ' character for example.
   * @param handler The callback when the link is called.
   * @param options Options for the link matcher.
   * @return The ID of the new matcher, this can be used to deregister.
   */
  public registerLinkMatcher(regex: RegExp, handler: LinkMatcherHandler, options: ILinkMatcherOptions = {}): number {
    if (!handler) {
      throw new Error('handler must be defined');
    }
    const matcher: ILinkMatcher = {
      id: this._nextLinkMatcherId++,
      regex,
      handler,
      matchIndex: options.matchIndex,
      validationCallback: options.validationCallback,
      hoverTooltipCallback: options.tooltipCallback,
      hoverLeaveCallback: options.leaveCallback,
      willLinkActivate: options.willLinkActivate,
      priority: options.priority || 0
    };
    this._addLinkMatcherToList(matcher);
    return matcher.id;
  }

  /**
   * Inserts a link matcher to the list in the correct position based on the
   * priority of each link matcher. New link matchers of equal priority are
   * considered after older link matchers.
   * @param matcher The link matcher to be added.
   */
  private _addLinkMatcherToList(matcher: ILinkMatcher): void {
    if (this._linkMatchers.length === 0) {
      this._linkMatchers.push(matcher);
      return;
    }

    for (let i = this._linkMatchers.length - 1; i >= 0; i--) {
      if (matcher.priority <= this._linkMatchers[i].priority) {
        this._linkMatchers.splice(i + 1, 0, matcher);
        return;
      }
    }

    this._linkMatchers.splice(0, 0, matcher);
  }

  /**
   * Deregisters a link matcher if it has been registered.
   * @param matcherId The link matcher's ID (returned after register)
   * @return Whether a link matcher was found and deregistered.
   */
  public deregisterLinkMatcher(matcherId: number): boolean {
    for (let i = 0; i < this._linkMatchers.length; i++) {
      if (this._linkMatchers[i].id === matcherId) {
        this._linkMatchers.splice(i, 1);
        return true;
      }
    }
    return false;
  }

  /**
   * Linkifies a row given a specific handler.
   * @param rowIndex The row index to linkify (absolute index).
   * @param text string content of the unwrapped row.
   * @param matcher The link matcher for this line.
   */
  private _doLinkifyRow(rowIndex: number, text: string, matcher: ILinkMatcher): void {
    // clone regex to do a global search on text
    const rex = new RegExp(matcher.regex.source, matcher.regex.flags + 'g');
    let match;
    let stringIndex = -1;
    while ((match = rex.exec(text)) !== null) {
      const uri = match[typeof matcher.matchIndex !== 'number' ? 0 : matcher.matchIndex];
      if (!uri) {
        // something matched but does not comply with the given matchIndex
        // since this is most likely a bug the regex itself we simply do nothing here
        // DEBUG: print match and throw
        if ((<any>this._terminal).debug) {
          console.log({match, matcher});
          throw new Error('match found without corresponding matchIndex');
        }
        break;
      }

      // Get index, match.index is for the outer match which includes negated chars
      // therefore we cannot use match.index directly, instead we search the position
      // of the match group in text again
      // also correct regex and string search offsets for the next loop run
      stringIndex = text.indexOf(uri, stringIndex + 1);
      rex.lastIndex = stringIndex + uri.length;
      if (stringIndex < 0) {
        // invalid stringIndex (should not have happened)
        break;
      }

      // get the buffer index as [absolute row, col] for the match
      const bufferIndex = this._terminal.buffer.stringIndexToBufferIndex(rowIndex, stringIndex);
      if (bufferIndex[0] < 0) {
        // invalid bufferIndex (should not have happened)
        break;
      }

      const line = this._terminal.buffer.lines.get(bufferIndex[0]);
      const attr = line.getFg(bufferIndex[1]);
      let fg: number | undefined;
      if (attr) {
        fg = (attr >> 9) & 0x1ff;
      }

      if (matcher.validationCallback) {
        matcher.validationCallback(uri, isValid => {
          // Discard link if the line has already changed
          if (this._rowsTimeoutId) {
            return;
          }
          if (isValid) {
            this._addLink(bufferIndex[1], bufferIndex[0] - this._terminal.buffer.ydisp, uri, matcher, fg);
          }
        });
      } else {
        this._addLink(bufferIndex[1], bufferIndex[0] - this._terminal.buffer.ydisp, uri, matcher, fg);
      }
    }
  }

  /**
   * Registers a link to the mouse zone manager.
   * @param x The column the link starts.
   * @param y The row the link is on.
   * @param uri The URI of the link.
   * @param matcher The link matcher for the link.
   * @param fg The link color for hover event.
   */
  private _addLink(x: number, y: number, uri: string, matcher: ILinkMatcher, fg: number): void {
    const width = getStringCellWidth(uri);
    const x1 = x % this._terminal.cols;
    const y1 = y + Math.floor(x / this._terminal.cols);
    let x2 = (x1 + width) % this._terminal.cols;
    let y2 = y1 + Math.floor((x1 + width) / this._terminal.cols);
    if (x2 === 0) {
      x2 = this._terminal.cols;
      y2--;
    }

    this._mouseZoneManager.add(new MouseZone(
      x1 + 1,
      y1 + 1,
      x2 + 1,
      y2 + 1,
      e => {
        if (matcher.handler) {
          return matcher.handler(e, uri);
        }
        window.open(uri, '_blank');
      },
      () => {
        this._onLinkHover.fire(this._createLinkHoverEvent(x1, y1, x2, y2, fg));
        this._terminal.element.classList.add('xterm-cursor-pointer');
      },
      e => {
        this._onLinkTooltip.fire(this._createLinkHoverEvent(x1, y1, x2, y2, fg));
        if (matcher.hoverTooltipCallback) {
          matcher.hoverTooltipCallback(e, uri);
        }
      },
      () => {
        this._onLinkLeave.fire(this._createLinkHoverEvent(x1, y1, x2, y2, fg));
        this._terminal.element.classList.remove('xterm-cursor-pointer');
        if (matcher.hoverLeaveCallback) {
          matcher.hoverLeaveCallback();
        }
      },
      e => {
        if (matcher.willLinkActivate) {
          return matcher.willLinkActivate(e, uri);
        }
        return true;
      }
    ));
  }

  private _createLinkHoverEvent(x1: number, y1: number, x2: number, y2: number, fg: number): ILinkifierEvent {
    return { x1, y1, x2, y2, cols: this._terminal.cols, fg };
  }
}
