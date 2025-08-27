pipeline {
    agent none
    stages {
        stage('Matrix Build') {
            matrix {
                axes {
                    axis {
                        name 'IMAGE'
                        values 'qt5-20.04', 'qt6-22.04', 'qt6-24.04'
                    }
                    axis {
                        name 'BUILD_TYPE'
                        values 'Debug', 'Release'
                    }
                }
                stages {
                    stage('Build and Test') {
                        agent any
                        steps {
                            script {
                                docker.image("juzzlin/${IMAGE}:latest").inside('--privileged -t -v $WORKSPACE:/Argengine') {
                                    def buildDir = "build-${BUILD_TYPE.toLowerCase()}-${IMAGE}"
                                    sh "mkdir -p ${buildDir}"
                                    sh "cd ${buildDir} && cmake -GNinja -DCMAKE_BUILD_TYPE=${BUILD_TYPE} .."
                                    sh "cd ${buildDir} && cmake --build . && ctest"
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

