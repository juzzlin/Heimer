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
                sh "mkdir -p build-mxe"
                sh "cd build-mxe && cmake .."
                sh "cd build-mxe && make -j2"
            }
        }
        stage('Debug build for Linux') {
            steps {
                sh "mkdir -p build-debug"
                sh "cd build-debug && cmake -DCMAKE_BUILD_TYPE=Debug .."
                sh "cd build-debug && make -j2"
            }
        }
        stage('Release build and Debian package for Linux') {
            steps {
                sh "mkdir -p build"
                sh "cd build && cmake .."
                sh "cd build && cpack -G DEB -D CMAKE_BUILD_TYPE=Release"
            }
        }
        stage('Run tests') {
            steps {
                sh "cd build && ctest"
            }
        }
    }
    post {
        always {
            archiveArtifacts artifacts: 'build/*.deb', fingerprint: true
        }
    }
}

