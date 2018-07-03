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
        stage('Debug build for Linux') {
            steps {
                sh "mkdir -p build-cmake-debug"
                sh "cd build-cmake-debug && cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j2"
            }
        }
        stage('Release build and Debian package for Linux') {
            steps {
                sh "mkdir -p build-cmake"
                sh "cd build-cmake && cmake .. && cpack -G DEB -D CMAKE_BUILD_TYPE=Release"
            }
        }
        stage('Run tests') {
            steps {
                sh "cd build-cmake && ctest"
            }
        }
    }
}

