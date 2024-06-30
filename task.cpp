#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex mtx;

class WorkerBee {
public:
    WorkerBee(double v, double startX, double startY, double endX, double endY)
        : speed(v), x(startX), y(startY), startX(startX), startY(startY), endX(endX), endY(endY), returning(false) {}

    void update(double timeStep) {
        double distance = std::sqrt(std::pow(endX - x, 2) + std::pow(endY - y, 2));
        double travelDistance = speed * timeStep;

        if (distance <= travelDistance) {
            if (returning) {
                x = startX;
                y = startY;
                returning = false;
            } else {
                x = endX;
                y = endY;
                returning = true;
            }
        } else {
            double ratio = travelDistance / distance;
            x += (endX - x) * ratio;
            y += (endY - y) * ratio;
        }
    }

    void printPosition() const {
        std::lock_guard<std::mutex> guard(mtx);
        std::cout << "Worker Bee Position: (" << x << ", " << y << ")\n";
    }

private:
    double x, y;
    double speed;
    double startX, startY, endX, endY;
    bool returning;
};

class DroneBee {
public:
    DroneBee(double v, double startX, double startY)
        : speed(v), x(startX), y(startY), directionX(1), directionY(0) {
        std::srand(std::time(nullptr));
    }

    void update(double timeStep) {
        if (std::rand() % 100 < 10) {
            double angle = (std::rand() % 360) * M_PI / 180;
            directionX = std::cos(angle);
            directionY = std::sin(angle);
        }

        x += speed * directionX * timeStep;
        y += speed * directionY * timeStep;
    }

    void printPosition() const {
        std::lock_guard<std::mutex> guard(mtx);
        std::cout << "Drone Bee Position: (" << x << ", " << y << ")\n";
    }

private:
    double x, y;
    double speed;
    double directionX, directionY;
};

void workerBeeThread(WorkerBee& bee, double timeStep, double totalTime) {
    for (double time = 0; time <= totalTime; time += timeStep) {
        bee.update(timeStep);
        bee.printPosition();
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(timeStep * 1000)));
    }
}

void droneBeeThread(DroneBee& bee, double timeStep, double totalTime) {
    for (double time = 0; time <= totalTime; time += timeStep) {
        bee.update(timeStep);
        bee.printPosition();
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(timeStep * 1000)));
    }
}

int main() {
    double speed = 1.0;
    double startX = 5.0;
    double startY = 5.0;
    double endX = 0.0;
    double endY = 0.0;
    double timeStep = 1.0;
    double totalTime = 20.0;

    WorkerBee worker(speed, startX, startY, endX, endY);
    DroneBee drone(speed, startX, startY);

    std::thread workerT([&worker, timeStep, totalTime]() {
        workerBeeThread(worker, timeStep, totalTime);
    });

    std::thread droneT([&drone, timeStep, totalTime]() {
        droneBeeThread(drone, timeStep, totalTime);
    });

    workerT.join();
    droneT.join();

    return 0;
}
