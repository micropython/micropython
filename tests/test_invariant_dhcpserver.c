#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Include the actual production header
#include "shared/netutils/dhcpserver.h"

START_TEST(test_dhcp_memcpy_bounds)
{
    // Invariant: DHCP packet processing must not overflow destination buffers
    // regardless of input packet size
    
    // Test payloads: exploit case, boundary case, valid case
    struct {
        const char *description;
        uint8_t *packet_data;
        size_t packet_len;
        size_t dest_buffer_size;
    } test_cases[] = {
        {
            "Exploit: oversized packet exceeding buffer",
            (uint8_t *)"A",  // Content doesn't matter for overflow
            1500,            // Typical MTU, likely > buffer size
            300              // Actual buffer size from dhcpserver.c
        },
        {
            "Boundary: packet size equals buffer",
            (uint8_t *)"B",
            300,             // Exactly buffer size
            300
        },
        {
            "Valid: normal DHCP packet",
            (uint8_t *)"C",
            100,             // Well within buffer
            300
        }
    };
    
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    
    for (int i = 0; i < num_cases; i++) {
        // Create destination buffer with canaries to detect overflow
        uint8_t *dest_buffer = malloc(test_cases[i].dest_buffer_size + 32);
        uint8_t *canary_start = dest_buffer;
        uint8_t *canary_end = dest_buffer + test_cases[i].dest_buffer_size + 16;
        
        // Set up canaries
        memset(canary_start, 0xAA, 16);
        memset(dest_buffer + test_cases[i].dest_buffer_size, 0xBB, 16);
        memset(canary_end, 0xCC, 16);
        
        // Clear main buffer area
        memset(dest_buffer + 16, 0, test_cases[i].dest_buffer_size);
        
        // Simulate the vulnerable memcpy operation that would be in process_dhcp_packet
        // We're testing that the actual function doesn't overflow, not the memcpy directly
        int result = process_dhcp_packet(dest_buffer + 16, test_cases[i].packet_data, 
                                        test_cases[i].packet_len);
        
        // Check canaries - if they're corrupted, overflow occurred
        int canary1_intact = memcmp(canary_start, "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"
                                                "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA", 16) == 0;
        int canary2_intact = memcmp(canary_end, "\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC"
                                              "\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC", 16) == 0;
        
        // Property: Canaries must remain intact regardless of input size
        ck_assert_msg(canary1_intact && canary2_intact,
                     "Buffer overflow detected for test case: %s", 
                     test_cases[i].description);
        
        free(dest_buffer);
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_dhcp_memcpy_bounds);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}