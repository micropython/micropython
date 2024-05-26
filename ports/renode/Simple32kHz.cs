//
// Copyright (c) 2010-2022 Antmicro
// Copyright (c) 2011-2015 Realtime Embedded
//
// This file is licensed under the MIT License.
// Full license text is available in 'licenses/MIT.txt'.
//
// This is modified for CircuitPython to tick at 32kHz like a slow external
// crystal would.
using System;
using Antmicro.Renode.Core;
using Antmicro.Renode.Peripherals.Bus;
using Antmicro.Renode.Time;
using Antmicro.Renode.Logging;
using System.Threading;

namespace Antmicro.Renode.Peripherals.Timers
{
    public class Simple32kHz : IDoubleWordPeripheral, IKnownSize
    {
        public long Size { get { return 0x4; } }

        public Simple32kHz(IMachine machine)
        {
            machine.ClockSource.AddClockEntry(new ClockEntry(1, 32768, OnTick, this, String.Empty));
        }

        public virtual uint ReadDoubleWord(long offset)
        {
            return (uint)counter;
        }

        public virtual void WriteDoubleWord(long offset, uint value)
        {
            this.LogUnhandledWrite(offset, value);
        }

        public virtual void Reset()
        {
            Interlocked.Exchange(ref counter, 0);
        }

        private void OnTick()
        {
            Interlocked.Increment(ref counter);
        }

        private int counter;
    }
}
