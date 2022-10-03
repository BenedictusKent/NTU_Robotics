#include "Aria.h"
using namespace std;

const int ACCVAL = 50;
const int ROTVAL = 10;
const int MAXVEL = 300;
const int MAXROTVEL = 50;
const int TOTALSONARNUMBER = 16;

ArRobot robot;
ArSonarDevice sonar;

void Forward()
{
	float currentVelocity = robot.getVel();
	float setVelocity = currentVelocity + ACCVAL;
	if(setVelocity > MAXVEL)
		setVelocity = MAXVEL;
	robot.setVel(setVelocity);
	cout << "velocity: " << setVelocity << endl;
}

void Left()
{
	float currentRotationalVel = robot.getRotVel();
	float setRotationalVel = currentRotationalVel + ROTVAL;
	if(setRotationalVel > MAXROTVEL)
		setRotationalVel = MAXROTVEL;
	robot.setRotVel(setRotationalVel);
	cout << "rotational: " << setRotationalVel << endl;
}

void Back()
{
	float currentVelocity = robot.getVel();
	float setVelocity = currentVelocity - ACCVAL;
	if(setVelocity < -MAXVEL)
		setVelocity = -MAXVEL;
	robot.setVel(setVelocity);
	cout << "velocity: " << setVelocity << endl;
}

void Right()
{
	float currentRotationalVel = robot.getRotVel();
	float setRotationalVel = currentRotationalVel - ROTVAL;
	if (setRotationalVel < -MAXROTVEL)
		setRotationalVel = -MAXROTVEL;
	robot.setRotVel(setRotationalVel);
	cout << "rotational: " << setRotationalVel << endl;
}

void Stop()
{
	robot.setVel(0);
	robot.setRotVel(0);
}

void CollisionAvoidance()
{
	unsigned int reading[TOTALSONARNUMBER] = {};

	for(int i=0; i<TOTALSONARNUMBER; ++i)
	{
		ArSensorReading sensor = *robot.getSonarReading(i);
		reading[i] = sensor.getRange();
	}

	for(int i=0; i<TOTALSONARNUMBER; ++i)
	{
		if(reading[i] < 500)
		{
			robot.setVel(0);
			robot.setRotVel(0);
		}
	}
}

int main(int argc, char **argv)
{
	ArGlobalFunctor forwardMove(&Forward);
	ArGlobalFunctor leftMove(&Left);
	ArGlobalFunctor backMove(&Back);
	ArGlobalFunctor rightMove(&Right);
	ArGlobalFunctor stopMove(&Stop);

	robot.addRangeDevice(&sonar);
	Aria::init();

	ArSimpleConnector connector(&argc,argv);

	if (!connector.connectRobot(&robot)){
		printf("Could not connect to robot... exiting\n");
		Aria::shutdown();
		Aria::exit(1);
	}

	robot.comInt(ArCommands::ENABLE, 1);
	robot.runAsync(false);

	// Used to perform actions when keyboard keys are pressed
	ArKeyHandler keyHandler;
	keyHandler.addKeyHandler('w', &forwardMove);
	keyHandler.addKeyHandler('a', &leftMove);
	keyHandler.addKeyHandler('s', &backMove);
	keyHandler.addKeyHandler('d', &rightMove);
	keyHandler.addKeyHandler('`', &stopMove);
	Aria::setKeyHandler(&keyHandler);
	robot.attachKeyHandler(&keyHandler);
	printf("You may press escape to exit\n");

	robot.lock();

	robot.unlock();

	while(true){
		printf("%f %f %f\n", robot.getX(), robot.getY(), robot.getTh());
		ArUtil::sleep(300);
		CollisionAvoidance();
	}

	Aria::shutdown();
	Aria::exit(0);
}
