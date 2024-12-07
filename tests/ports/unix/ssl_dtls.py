# Very basic test of ssl.SSLContext DTLS.

try:
    import ssl
    import ssl.PROTOCOL_DTLS_CLIENT
    import ssl.PROTOCOL_DTLS_SERVER
except ImportError:
    print("SKIP")
    raise SystemExit
except NameError:
    print("SKIP")
    raise SystemExit

# Test constructing with valid arguments
dtls_client = ssl.SSLContext(ssl.PROTOCOL_DTLS_CLIENT)
dtls_server = ssl.SSLContext(ssl.PROTOCOL_DTLS_SERVER)

# Test exception handling paths
try:
    # Create a context with invalid protocol to trigger error
    invalid_context = ssl.SSLContext(0xFF)
    print("FAIL: Should have raised ValueError")
except ValueError:
    print("OK: ValueError raised for invalid protocol")


# Test certificate verification callback error path
try:

    def bad_verify_callback(cert, depth):
        raise ValueError("Test callback error")

    context = ssl.SSLContext(ssl.PROTOCOL_DTLS_CLIENT)
    context.verify_callback = bad_verify_callback
    # Need to trigger the callback - try to connect to any server
    # This should raise an exception from the callback
    context.get_ciphers()  # This should trigger the callback
    print("FAIL: Should have raised ValueError from callback")
except ValueError as e:
    if str(e) == "Test callback error":
        print("OK: ValueError raised from callback")
    else:
        print("FAIL: Unexpected ValueError:", str(e))

# Test DTLS timing functionality
import time

def test_dtls_timing():
    try:
        context = ssl.SSLContext(ssl.PROTOCOL_DTLS_CLIENT)
        sock = context.wrap_socket(None, server_side=False, do_handshake_on_connect=False)
        
        # Force set the timing values for testing
        # These are normally set by mbedtls during handshake
        sock._timer_int_ms = 100  # intermediate timeout
        sock._timer_fin_ms = 200  # final timeout
        sock._timer_start_ms = time.ticks_ms()  # start time
        
        # Sleep to test different timing scenarios
        time.sleep_ms(50)  # Should return 0 (no timeout)
        print("Timer < int_ms:", sock._get_timer_delay() == 0)
        
        time.sleep_ms(100)  # Should return 1 (intermediate timeout)
        print("Timer >= int_ms:", sock._get_timer_delay() == 1)
        
        time.sleep_ms(100)  # Should return 2 (final timeout)
        print("Timer >= fin_ms:", sock._get_timer_delay() == 2)
        
        # Test with timer disabled
        sock._timer_fin_ms = 0
        print("Timer disabled:", sock._get_timer_delay() == -1)
        
    except Exception as e:
        print("Error in DTLS timing test:", str(e))

# Run the timing test
test_dtls_timing()
