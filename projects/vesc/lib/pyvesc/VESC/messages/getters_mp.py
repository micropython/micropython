from pyvesc.protocol.base_mp import VESCMessage
from pyvesc.VESC.messages import VedderCmd_mp


def GetVersion_mp():
    """ Gets version fields
    """
    id = VedderCmd_mp.COMM_FW_VERSION

    fields = [
            ('comm_fw_version', 'b', 0),
            ('fw_version_major', 'b', 0),
            ('fw_version_minor', 'b', 0)
    ]

    my_msg = VESCMessage("GetVersion", fields, id)
    return my_msg


def GetValues_mp():
    """ Gets internal sensor data
    """
    id = VedderCmd_mp.COMM_GET_VALUES

    fields = [
        ('temp_fet', 'h', 10),
        ('temp_motor', 'h', 10),
        ('avg_motor_current', 'i', 100),
        ('avg_input_current', 'i', 100),
        ('avg_id', 'i', 100),
        ('avg_iq', 'i', 100),
        ('duty_cycle_now', 'h', 1000),
        ('rpm', 'i', 1),
        ('v_in', 'h', 10),
        ('amp_hours', 'i', 10000),
        ('amp_hours_charged', 'i', 10000),
        ('watt_hours', 'i', 10000),
        ('watt_hours_charged', 'i', 10000),
        ('tachometer', 'i', 1),
        ('tachometer_abs', 'i', 1),
        ('mc_fault_code', 'b', 0),
        ('pid_pos_now', 'i', 1000000),
        ('app_controller_id', 'b', 0),
        ('time_ms', 'i', 1),
    ]
    my_msg = VESCMessage("GetValues", fields, id)
    return my_msg


