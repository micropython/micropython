#Source: https://github.com/pycom/pycom-modbus/tree/master/uModbus (2018-07-16)
#This file has been modified and differ from its source version.


import uModBusFunctions as functions
import uModBusConst as Const
import struct
import socket
import random

class uModBusTCP:

    def __init__(self, slave_ip, slave_port=502, timeout=5):
        self._sock = socket.socket()
        self._sock.connect(socket.getaddrinfo(slave_ip, slave_port)[0][-1])
        self._sock.settimeout(timeout)

    def _create_mbap_hdr(self, slave_id, modbus_pdu):
        trans_id = random.randint(0, 65535) & 0xFFFF
        mbap_hdr = struct.pack('>HHHB', trans_id, 0, len(modbus_pdu) + 1, slave_id)

        return mbap_hdr, trans_id

    def _bytes_to_bool(self, byte_list):
        bool_list = []
        for index, byte in enumerate(byte_list):
            bool_list.extend([bool(byte & (1 << n)) for n in range(8)])

        return bool_list

    def _to_short(self, byte_array, signed=True):
        response_quantity = int(len(byte_array) / 2)
        fmt = '>' + (('h' if signed else 'H') * response_quantity)

        return struct.unpack(fmt, byte_array)

    def _validate_resp_hdr(self, response, trans_id, slave_id, function_code, count=False):
        rec_tid, rec_pid, rec_len, rec_uid, rec_fc = struct.unpack('>HHHBB', response[:Const.MBAP_HDR_LENGTH + 1])
        if (trans_id != rec_tid):
            raise ValueError('wrong transaction Id')

        if (rec_pid != 0):
            raise ValueError('invalid protocol Id')

        if (slave_id != rec_uid):
            raise ValueError('wrong slave Id')

        if (rec_fc == (function_code + Const.ERROR_BIAS)):
            raise ValueError('slave returned exception code: {:d}'.format(rec_fc))

        hdr_length = (Const.MBAP_HDR_LENGTH + 2) if count else (Const.MBAP_HDR_LENGTH + 1)

        return response[hdr_length:]

    def _send_receive(self, slave_id, modbus_pdu, count):
        mbap_hdr, trans_id = self._create_mbap_hdr(slave_id, modbus_pdu)
        self._sock.send(mbap_hdr + modbus_pdu)

        response = self._sock.recv(256)
        modbus_data = self._validate_resp_hdr(response, trans_id, slave_id, modbus_pdu[0], count)

        return modbus_data

    def read_coils(self, slave_addr, starting_addr, coil_qty):
        modbus_pdu = functions.read_coils(starting_addr, coil_qty)

        response = self._send_receive(slave_addr, modbus_pdu, True)
        status_pdu = self._bytes_to_bool(response)

        return status_pdu

    def read_discrete_inputs(self, slave_addr, starting_addr, input_qty):
        modbus_pdu = functions.read_discrete_inputs(starting_addr, input_qty)

        response = self._send_receive(slave_addr, modbus_pdu, True)
        status_pdu = self._bytes_to_bool(response)

        return status_pdu

    def read_holding_registers(self, slave_addr, starting_addr, register_qty, signed = True):
        modbus_pdu = functions.read_holding_registers(starting_addr, register_qty)

        response = self._send_receive(slave_addr, modbus_pdu, True)
        register_value = self._to_short(response, signed)

        return register_value

    def read_input_registers(self, slave_addr, starting_address, register_quantity, signed = True):
        modbus_pdu = functions.read_input_registers(starting_address, register_quantity)

        response = self._send_receive(slave_addr, modbus_pdu, True)
        register_value = self._to_short(response, signed)

        return register_value

    def write_single_coil(self, slave_addr, output_address, output_value):
        modbus_pdu = functions.write_single_coil(output_address, output_value)

        response = self._send_receive(slave_addr, modbus_pdu, False)
        operation_status = functions.validate_resp_data(response, Const.WRITE_SINGLE_COIL,
                                                        output_address, value=output_value, signed=False)

        return operation_status

    def write_single_register(self, slave_addr, register_address, register_value, signed=True):
        modbus_pdu = functions.write_single_register(register_address, register_value, signed)

        response = self._send_receive(slave_addr, modbus_pdu, False)
        operation_status = functions.validate_resp_data(response, Const.WRITE_SINGLE_REGISTER,
                                                        register_address, value=register_value, signed=signed)

        return operation_status

    def write_multiple_coils(self, slave_addr, starting_address, output_values):
        modbus_pdu = functions.write_multiple_coils(starting_address, output_values)

        response = self._send_receive(slave_addr, modbus_pdu, False)
        operation_status = functions.validate_resp_data(response, Const.WRITE_MULTIPLE_COILS,
                                                        starting_address, quantity=len(output_values))

        return operation_status

    def write_multiple_registers(self, slave_addr, starting_address, register_values, signed=True):
        modbus_pdu = functions.write_multiple_registers(starting_address, register_values, signed)

        response = self._send_receive(slave_addr, modbus_pdu, False)
        operation_status = functions.validate_resp_data(response, Const.WRITE_MULTIPLE_REGISTERS,
                                                        starting_address, quantity=len(register_values))

        return operation_status

    def close(self):
        self._sock.close()
