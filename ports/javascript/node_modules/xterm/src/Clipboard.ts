/**
 * Copyright (c) 2016 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { ITerminal, ISelectionManager } from './Types';

interface IWindow extends Window {
  clipboardData?: {
    getData(format: string): string;
    setData(format: string, data: string): void;
  };
}

declare var window: IWindow;

/**
 * Prepares text to be pasted into the terminal by normalizing the line endings
 * @param text The pasted text that needs processing before inserting into the terminal
 */
export function prepareTextForTerminal(text: string): string {
  return text.replace(/\r?\n/g, '\r');
}

/**
 * Bracket text for paste, if necessary, as per https://cirw.in/blog/bracketed-paste
 * @param text The pasted text to bracket
 */
export function bracketTextForPaste(text: string, bracketedPasteMode: boolean): string {
  if (bracketedPasteMode) {
    return '\x1b[200~' + text + '\x1b[201~';
  }
  return text;
}

/**
 * Binds copy functionality to the given terminal.
 * @param ev The original copy event to be handled
 */
export function copyHandler(ev: ClipboardEvent, term: ITerminal, selectionManager: ISelectionManager): void {
  if (term.browser.isMSIE) {
    window.clipboardData.setData('Text', selectionManager.selectionText);
  } else {
    ev.clipboardData.setData('text/plain', selectionManager.selectionText);
  }

  // Prevent or the original text will be copied.
  ev.preventDefault();
}

/**
 * Redirect the clipboard's data to the terminal's input handler.
 * @param ev The original paste event to be handled
 * @param term The terminal on which to apply the handled paste event
 */
export function pasteHandler(ev: ClipboardEvent, term: ITerminal): void {
  ev.stopPropagation();

  let text: string;

  const dispatchPaste = function(text: string): void {
    text = prepareTextForTerminal(text);
    text = bracketTextForPaste(text, term.bracketedPasteMode);
    term.handler(text);
    term.textarea.value = '';
    term.emit('paste', text);
    term.cancel(ev);
  };

  if (term.browser.isMSIE) {
    if (window.clipboardData) {
      text = window.clipboardData.getData('Text');
      dispatchPaste(text);
    }
  } else {
    if (ev.clipboardData) {
      text = ev.clipboardData.getData('text/plain');
      dispatchPaste(text);
    }
  }
}

/**
 * Moves the textarea under the mouse cursor and focuses it.
 * @param ev The original right click event to be handled.
 * @param textarea The terminal's textarea.
 */
export function moveTextAreaUnderMouseCursor(ev: MouseEvent, term: ITerminal): void {

  // Calculate textarea position relative to the screen element
  const pos = term.screenElement.getBoundingClientRect();
  const left = ev.clientX - pos.left - 10;
  const top = ev.clientY - pos.top - 10;

  // Bring textarea at the cursor position
  term.textarea.style.position = 'absolute';
  term.textarea.style.width = '20px';
  term.textarea.style.height = '20px';
  term.textarea.style.left = `${left}px`;
  term.textarea.style.top = `${top}px`;
  term.textarea.style.zIndex = '1000';

  term.textarea.focus();

  // Reset the terminal textarea's styling
  // Timeout needs to be long enough for click event to be handled.
  setTimeout(() => {
    term.textarea.style.position = null;
    term.textarea.style.width = null;
    term.textarea.style.height = null;
    term.textarea.style.left = null;
    term.textarea.style.top = null;
    term.textarea.style.zIndex = null;
  }, 200);
}

/**
 * Bind to right-click event and allow right-click copy and paste.
 * @param ev The original right click event to be handled.
 * @param textarea The terminal's textarea.
 * @param selectionManager The terminal's selection manager.
 * @param shouldSelectWord If true and there is no selection the current word will be selected
 */
export function rightClickHandler(ev: MouseEvent, term: ITerminal, selectionManager: ISelectionManager, shouldSelectWord: boolean): void {
  moveTextAreaUnderMouseCursor(ev, term);

  if (shouldSelectWord && !selectionManager.isClickInSelection(ev)) {
    selectionManager.selectWordAtCursor(ev);
  }

  // Get textarea ready to copy from the context menu
  term.textarea.value = selectionManager.selectionText;
  term.textarea.select();
}
