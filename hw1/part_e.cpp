#include "Aria.h"
#define _USE_MATH_DEFINES
using namespace std;

ArRobot robot;
ArSonarDevice sonar;

void Navigator(ArPose *target)
{
	double angle = robot.findAngleTo(*target);
	double distance = robot.findDistanceTo(*target);
	robot.setHeading(angle);
	while(!robot.isHeadingDone()){}
	robot.move(distance);
	while(!robot.isMoveDone()){}
	robot.setHeading(target->getTh());
	while(!robot.isHeadingDone()){}
	printf("End pos: %f %f %f\n", robot.getX(), robot.getY(), robot.getTh());
}

int main(int argc, char **argv)
{
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

	ArPose *initpose = new ArPose(5090, 3580, 3093.97);
	robot.moveTo(*initpose);
	char ans;
	do {
		cout << "Input specific target:" << endl;
		double x, y, theta;
		cin >> x >> y >> theta;
		x *= 1000;
		y *= 1000;
		theta = theta * 180 / M_PI;
		ArPose *pose = new ArPose(x, y, theta);
		Navigator(pose);
		ArUtil::sleep(300);
		cout << "Set next target? [y/n] ";
		cin >> ans;
	} while(ans == 'y');

	Aria::shutdown();
	Aria::exit(0);
}
