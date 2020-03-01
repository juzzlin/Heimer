pipeline {
    agent any
    stages {
        stage('CMake Debug build') {
            agent {
                docker {
                    image 'juzzlin/qt5-18.04:latest'
                    args '--privileged -t -v $WORKSPACE:/Argengine'
                }
            }
            steps {
                sh "mkdir -p build-debug"
                sh "cd build-debug && cmake -DCMAKE_BUILD_TYPE=Debug .."
                sh "cd build-debug && cmake --build . --target all -- -j3 && ctest"
            }
        }
        stage('CMake Release build') {
            agent {
                docker {
                    image 'juzzlin/qt5-18.04:latest'
                    args '--privileged -t -v $WORKSPACE:/Argengine'
                }
            }
            steps {
                sh "mkdir -p build-release"
                sh "cd build-release && cmake -DCMAKE_BUILD_TYPE=Release .."
                sh "cd build-release && cmake --build . --target all -- -j3 && ctest"
            }
        }
    }
}

