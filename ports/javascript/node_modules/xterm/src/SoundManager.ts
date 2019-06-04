/**
 * Copyright (c) 2018 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { ITerminal, ISoundManager } from './Types';

// Source: https://freesound.org/people/altemark/sounds/45759/
// This sound is released under the Creative Commons Attribution 3.0 Unported
// (CC BY 3.0) license. It was created by 'altemark'. No modifications have been
// made, apart from the conversion to base64.
export const DEFAULT_BELL_SOUND = 'data:audio/wav;base64,UklGRigBAABXQVZFZm10IBAAAAABAAEARKwAAIhYAQACABAAZGF0YQQBAADpAFgCwAMlBZoG/wdmCcoKRAypDQ8PbRDBEQQTOxRtFYcWlBePGIUZXhoiG88bcBz7HHIdzh0WHlMeZx51HmkeUx4WHs8dah0AHXwc3hs9G4saxRnyGBIYGBcQFv8U4RPAEoYRQBACD70NWwwHC6gJOwjWBloF7gOBAhABkf8b/qv8R/ve+Xf4Ife79W/0JfPZ8Z/wde9N7ijtE+wU6xvqM+lb6H7nw+YX5mrlxuQz5Mzje+Ma49fioeKD4nXiYeJy4pHitOL04j/jn+MN5IPkFOWs5U3mDefM55/ogOl36m7rdOyE7abuyu8D8Unyj/Pg9D/2qfcb+Yn6/vuK/Qj/lAAlAg==';

export class SoundManager implements ISoundManager {
  private static _audioContext: AudioContext;

  static get audioContext(): AudioContext | null {
    if (!SoundManager._audioContext) {
      const audioContextCtor: typeof AudioContext = (<any>window).AudioContext || (<any>window).webkitAudioContext;
      if (!audioContextCtor) {
        console.warn('Web Audio API is not supported by this browser. Consider upgrading to the latest version');
        return null;
      }
      SoundManager._audioContext = new audioContextCtor();
    }
    return SoundManager._audioContext;
  }

  constructor(
    private _terminal: ITerminal
  ) {
  }

  public playBellSound(): void {
    const ctx = SoundManager.audioContext;
    if (!ctx) {
      return;
    }
    const bellAudioSource = ctx.createBufferSource();
    ctx.decodeAudioData(this._base64ToArrayBuffer(this._removeMimeType(this._terminal.options.bellSound)), (buffer) => {
      bellAudioSource.buffer = buffer;
      bellAudioSource.connect(ctx.destination);
      bellAudioSource.start(0);
    });
  }

  private _base64ToArrayBuffer(base64: string): ArrayBuffer {
    const binaryString = window.atob(base64);
    const len = binaryString.length;
    const bytes = new Uint8Array(len);

    for (let i = 0; i < len; i++) {
      bytes[i] = binaryString.charCodeAt(i);
    }

    return bytes.buffer;
  }

  private _removeMimeType(dataURI: string): string {
    // Split the input to get the mime-type and the data itself
    const splitUri = dataURI.split(',');

    // Return only the data
    return splitUri[1];
  }
}
