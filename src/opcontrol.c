/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

/*
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */

 const unsigned int motors[128] =
 {
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0, 21, 21, 21, 22, 22, 22, 23, 24, 24,
  25, 25, 25, 25, 26, 27, 27, 28, 28, 28,
  28, 29, 30, 30, 30, 31, 31, 32, 32, 32,
  33, 33, 34, 34, 35, 35, 35, 36, 36, 37,
  37, 37, 37, 38, 38, 39, 39, 39, 40, 40,
  41, 41, 42, 42, 43, 44, 44, 45, 45, 46,
  46, 47, 47, 48, 48, 49, 50, 50, 51, 52,
  52, 53, 54, 55, 56, 57, 57, 58, 59, 60,
  61, 62, 63, 64, 65, 66, 67, 67, 68, 70,
  71, 72, 72, 73, 74, 76, 77, 78, 79, 79,
  80, 81, 83, 84, 84, 86, 86, 87, 87, 88,
  88, 89, 89, 90, 90,127,127,127
 };

void driveSet(int left, int right);
void liftSet(int value);
void liftControl(int target);
int lin(int value);

void operatorControl() {
  int liftTarget = encoderGet(liftEnc);
	while (1) {
		// ---------------- Drive ---------------
		int joyY = joystickGetAnalog(1, 2);
		int joyX = -joystickGetAnalog(1, 4);
		int strafeX = joystickGetAnalog(1, 1);
		motorSet(frDrive, lin(joyY-strafeX-joyX));
		motorSet(mrDrive, lin(joyY-strafeX+joyX));
		motorSet(brDrive, -lin(joyY-strafeX+joyX));
		motorSet(flDrive, -lin(joyY+strafeX+joyX));
		motorSet(mlDrive, -lin(joyY+strafeX-joyX));
		motorSet(blDrive, lin(joyY+strafeX-joyX));

		//  ---------------- Lift ----------------
		bool liftUp = joystickGetDigital(1, 5, JOY_UP);
		bool liftDown = joystickGetDigital(1, 5, JOY_DOWN);
    if (joystickGetDigital(1, 7, JOY_DOWN)) encoderReset(liftEnc);
		if(liftUp || liftDown)
    {
      liftSet(127 * (liftUp - liftDown));
      liftTarget = encoderGet(liftEnc);
    }
    else liftControl(liftTarget);

		delay(20);
	}
}

int lin(int value)
{
	if (value > 127) value = 127;
	if (value < -127) value = -127;
	if (value < 0) return -motors[-value];
	return motors[value];
}

void liftControl(int target)
{
	int lift = encoderGet(liftEnc);
	int error = target - lift;
	if(error > 127) error = 127;
	if(error < -127) error = -127;
	liftSet(error * 0.7);
}

void liftSet(int power)
{
	motorSet(trLift, -power);
	motorSet(brLift, power);
	motorSet(tlLift, -power);
	motorSet(blLift, power);
}

void driveSet(int left, int right)
{
	right = lin(right);
	left = lin(left);
	motorSet(frDrive, right);
	motorSet(mrDrive, -right);
	motorSet(brDrive, right);
	motorSet(flDrive, left);
	motorSet(mlDrive, -left);
	motorSet(blDrive, left);
}
