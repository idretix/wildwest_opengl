#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        //TODO - Update the rest of camera parameters
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));

    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        if (direction == MOVE_FORWARD) {
            this->cameraTarget.z -= speed;
            this->cameraPosition.z -= speed;
        }
        else {
            if (direction == MOVE_BACKWARD) {
                this->cameraTarget.z += speed;
                this->cameraPosition.z += speed;
            }
            else {
                if (direction == MOVE_RIGHT) {
                    this->cameraTarget.x += speed;
                    this->cameraPosition.x += speed;
                }
                else {
                    if (direction == MOVE_LEFT) {
                        this->cameraTarget.x -= speed;
                        this->cameraPosition.x -= speed;
                    }
                    else {
                        if (direction == MOVE_UP) {
                            this->cameraTarget.y += speed;
                            this->cameraPosition.y += speed;
                        }
                        else {
                            if (direction == MOVE_DOWN) {
                                this->cameraTarget.y -= speed;
                                this->cameraPosition.y -= speed;
                            }
                        }
                    }
                }
            }
        }
    }
    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        this->yawAngle += yaw;
        this->pitchAngle += pitch;

        this->cameraTarget.x = this->cameraPosition.x + sin(this->yawAngle) * cos(this->pitchAngle);
        this->cameraTarget.z = this->cameraPosition.z - cos(this->yawAngle) * cos(this->pitchAngle);

        this->cameraTarget.y = this->cameraPosition.y + sin(this->pitchAngle);

        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    }
}