typedef struct {
  const char* name;
  uint16_t code;
} GATT;

GATT services[] = {
  { "Generic Access",                          0x1800 },
  { "Alert Notification Service",              0x1811 },
  { "Automation IO",                           0x1815 },
  { "Battery Service",                         0x180F },
  { "Blood Pressure",                          0x1810 },
  { "Body Composition",                        0x181B },
  { "Bond Management",                         0x181E },
  { "Continuous Glucose Monitoring",           0x181F },
  { "Current Time Service",                    0x1805 },
  { "Cycling Power",                           0x1818 },
  { "Cycling Speed and Cadence",               0x1816 },
  { "Device Information",                      0x180A },
  { "Environmental Sensing",                   0x181A },
  { "Fitness Machine",                         0x1826 },
  { "Generic Attribute",                       0x1801 },
  { "Glucose",                                 0x1808 },
  { "Health Thermometer",                      0x1809 },
  { "Heart Rate",                              0x180D },
  { "HTTP Proxy",                              0x1823 },
  { "Human Interface Device",                  0x1812 },
  { "Immediate Alert",                         0x1802 },
  { "Indoor Positioning",                      0x1821 },
  { "Insulin Delivery",                        0x183A },
  { "Internet Protocol Support Service",       0x1820 },
  { "Link Loss",                               0x1803 },
  { "Location and Navigation",                 0x1819 },
  { "Mesh Provisioning Service",               0x1827 },
  { "Mesh Proxy Service",                      0x1828 },
  { "Next DST Change Service",                 0x1807 },
  { "Object Transfer Service",                 0x1825 },
  { "Phone Alert Status Service",              0x180E },
  { "Pulse Oximeter Service",                  0x1822 },
  { "Reconnection Configuration",              0x1829 },
  { "Reference Time Update Service",           0x1806 },
  { "Running Speed and Cadence",               0x1814 },
  { "Scan Parameters",                         0x1813 },
  { "Transport Discovery",                     0x1824 },
  { "Tx Power",                                0x1804 },
  { "User Data",                               0x181C },
  { "Weight Scale",                            0x181D }
};

GATT get_service_name(uint16_t code) {
  GATT result;
  result.code = code;
  result.name = "Unknown";
  for (uint16_t i=0; i<sizeof(services); i++) {
    if (services[i].code == code) {
      result.code = services[i].code;
      result.name = services[i].name;
      break;
    }
  }
  return result;
}
