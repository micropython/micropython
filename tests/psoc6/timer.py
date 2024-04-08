from machine import Timer
import uasyncio as asyncio
interrupt_triggered = False  # Flag to indicate interrupt triggered

def call(timer):
    global interrupt_triggered
    interrupt_triggered = True

async def print_message_thread():
    global interrupt_triggered
    while True:
        if interrupt_triggered:
            print("Interrupt triggered")
            interrupt_triggered = False  # Reset the flag
        await asyncio.sleep(0.1)  # Use asyncio.sleep instead of time.sleep

async def main():
    print("Oneshot Timer Execution")
    tim = Timer(0, period=1000, mode=Timer.ONE_SHOT, callback=call)
    try:
        logger_task = asyncio.create_task(print_message_thread())
        await asyncio.sleep(5)  # Wait for 5 seconds
        logger_task.cancel()  # Cancel the logger task
    finally:
        tim.deinit()  # Deinitialize the timer
    print("Periodic Timer Execution")
    tim = Timer(0, period=1000, mode=Timer.PERIODIC, callback=call)
    try:
        logger_task = asyncio.create_task(print_message_thread())
        await asyncio.sleep(5)  # Wait for 5 seconds
        logger_task.cancel()  # Cancel the logger task
    finally:
        tim.deinit()  # Deinitialize the timer
        
if __name__ == "__main__":
    print("Timer Test")
    asyncio.run(main())