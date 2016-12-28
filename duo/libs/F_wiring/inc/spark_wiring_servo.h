/**
 ******************************************************************************
 * @file    spark_wiring_servo.h
 * @author  Zach Supalla
 * @version V1.0.0
 * @date    06-December-2013
 * @brief   Header for spark_wiring_servo.h module
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.
  Copyright (c) 2010 LeafLabs, LLC.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
  ******************************************************************************
 */

#ifndef _SERVO_H_
#define _SERVO_H_

#include "spark_wiring.h"

/*
 * Note on Arduino compatibility:
 *
 * In the Arduino implementation, PWM is done "by hand" in the sense
 * that timer channels are hijacked in groups and an ISR is set which
 * toggles Servo::attach()ed pins using digitalWrite().
 *
 * While this scheme allows any pin to drive a servo, it chews up
 * cycles and complicates the programmer's notion of when a particular
 * timer channel will be in use.
 *
 * This implementation only allows Servo instances to attach() to pins
 * that already have a timer channel associated with them, and just
 * uses pwmWrite() to drive the wave.
 *
 * This introduces an incompatibility: while the Arduino
 * implementation of attach() returns the affected channel on success
 * and 0 on failure, this one returns true on success and false on
 * failure.
 *
 * RC Servos expect a pulse every 20ms.  Since periods are set for
 * entire timers, rather than individual channels, attach()ing a Servo
 * to a pin can interfere with other pins associated with the same
 * timer.  As always, your board's pin map is your friend.
 */

// Pin number of unattached pins
#define NOT_ATTACHED                    (-1)

// Default min/max pulse widths (in microseconds) and angles (in
// degrees).  Values chosen for Arduino compatibility.  These values
// are part of the public API; DO NOT CHANGE THEM.
#define SERVO_DEFAULT_MIN_PW            544
#define SERVO_DEFAULT_MAX_PW            2400
#define SERVO_DEFAULT_MIN_ANGLE         0
#define SERVO_DEFAULT_MAX_ANGLE         180

/** Class for interfacing with RC servomotors. */
class Servo {
public:
    /**
     * @brief Construct a new Servo instance.
     *
     * The new instance will not be attached to any pin.
     */
    Servo();

    /**
     * @brief Associate this instance with a servomotor whose input is
     *        connected to pin.
     *
     * If this instance is already attached to a pin, it will be
     * detached before being attached to the new pin. This function
     * doesn't detach any interrupt attached with the pin's timer
     * channel.
     *
     * @param pin Pin connected to the servo pulse wave input. This
     *            pin must be capable of PWM output.
     *
     * @param minPulseWidth Minimum pulse width to write to pin, in
     *                      microseconds.  This will be associated
     *                      with a minAngle degree angle.  Defaults to
     *                      SERVO_DEFAULT_MIN_PW = 544.
     *
     * @param maxPulseWidth Maximum pulse width to write to pin, in
     *                      microseconds.  This will be associated
     *                      with a maxAngle degree angle. Defaults to
     *                      SERVO_DEFAULT_MAX_PW = 2400.
     *
     * @param minAngle Target angle (in degrees) associated with
     *                 minPulseWidth.  Defaults to
     *                 SERVO_DEFAULT_MIN_ANGLE = 0.
     *
     * @param maxAngle Target angle (in degrees) associated with
     *                 maxPulseWidth.  Defaults to
     *                 SERVO_DEFAULT_MAX_ANGLE = 180.
     *
     * @sideeffect May set pinMode(pin, PWM).
     *
     * @return true if successful, false when pin doesn't support PWM.
     */
    bool attach(uint16_t pin,
                uint16_t minPulseWidth=SERVO_DEFAULT_MIN_PW,
                uint16_t maxPulseWidth=SERVO_DEFAULT_MAX_PW,
                int16_t minAngle=SERVO_DEFAULT_MIN_ANGLE,
                int16_t maxAngle=SERVO_DEFAULT_MAX_ANGLE);

    /**
     * @brief Check if this instance is attached to a servo.
     * @return true if this instance is attached to a servo, false otherwise.
     * @see Servo::attachedPin()
     */
    bool attached() const { return this->pin != NOT_ATTACHED; }

    /**
     * @brief Get the pin this instance is attached to.
     * @return Pin number if currently attached to a pin, NOT_ATTACHED
     *         otherwise.
     * @see Servo::attach()
     */
    int attachedPin() const { return this->pin; }

    /**
     * @brief Stop driving the servo pulse train.
     *
     * If not currently attached to a motor, this function has no effect.
     *
     * @return true if this call did anything, false otherwise.
     */
    bool detach();

    /**
     * @brief Set the servomotor target angle.
     *
     * @param angle Target angle, in degrees.  If the target angle is
     *              outside the range specified at attach() time, it
     *              will be clamped to lie in that range.
     *
     * @see Servo::attach()
     */
    void write(int angle);


    /**
     * Get the servomotor's target angle, in degrees.  This will
     * lie inside the range specified at attach() time.
     *
     * @see Servo::attach()
     */
    int read() const;

    /**
     * @brief Set the pulse width, in microseconds.
     *
     * @param pulseWidth Pulse width to send to the servomotor, in
     *                   microseconds. If outside of the range
     *                   specified at attach() time, it is clamped to
     *                   lie in that range.
     *
     * @see Servo::attach()
     */
    void writeMicroseconds(uint16_t pulseWidth);

    /**
     * Get the current pulse width, in microseconds.  This will
     * lie within the range specified at attach() time.
     *
     * @see Servo::attach()
     */
    uint16_t readMicroseconds() const;

    void setTrim(int trim) { this->trim = trim; }

private:
    int16_t pin;
    uint16_t minPW;
    uint16_t maxPW;
    int16_t minAngle;
    int16_t maxAngle;
    int trim;
    void resetFields(void);
};

#endif  /* _SERVO_H_ */
