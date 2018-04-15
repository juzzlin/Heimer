pipeline {
    agent any
    stages {
        stage('Build CMake Debug') {
            steps {
                sh "mkdir -p build-cmake-debug"
                sh "cd build-cmake-debug && cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j2"
            }
        }
        stage('Build CMake') {
            steps {
                sh "mkdir -p build-cmake"
                sh "cd build-cmake && cmake .. && make -j2"
            }
        }
        stage('Build QMake') {
            steps {
                sh "mkdir -p build-qmake"
                sh "cd build-qmake && qmake .. && make -j2"
            }
        }
        stage('Build Linux Packaging Release') {
            steps {
                sh "mkdir -p build-linux-packaging-release"
                sh "cd build-linux-packaging-release && cmake -DCMAKE_BUILD_TYPE=Release -DReleaseBuild=ON .. && make -j2"
            }
        }
        stage('Run tests') {
            steps {
                sh "cd build-cmake && ctest"
            }
        }
    }
}

