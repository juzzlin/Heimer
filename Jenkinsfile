pipeline {
    agent any
    stages {
        stage('Build for Windows in Docker with MXE') {
            agent {
                docker {
                    image 'juzzlin/mxe-qt5:latest'
                    args '--privileged -t -v $WORKSPACE:/heimer'
                }
            }
            steps {
                sh "mkdir -p build-cmake-mxe"
                sh "cd build-cmake-mxe && cmake .. && make -j2"
            }
        }
        stage('Build for Linux (CMake Debug)') {
            steps {
                sh "mkdir -p build-cmake-debug"
                sh "cd build-cmake-debug && cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j2"
            }
        }
        stage('Build for Linux (CMake Release)') {
            steps {
                sh "mkdir -p build-cmake"
                sh "cd build-cmake && cmake .. && make -j2"
            }
        }
        stage('Build for Linux (QMake)') {
            steps {
                sh "mkdir -p build-qmake"
                sh "cd build-qmake && qmake .. && make -j2"
            }
        }
        stage('Run tests') {
            steps {
                sh "cd build-cmake && ctest"
            }
        }
        stage('Build Debian package)') {
            steps {
                sh "cd build-cmake && cpack -G DEB -D CMAKE_BUILD_TYPE=Release"
            }
        }
    }
}

