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
                sh "cd build-mxe && cmake -D CMAKE_BUILD_TYPE=Release .. && make -j3"
                sh "export LANG=en_US.UTF-8 && export LC_ALL=en_US.UTF-8 && export PATH=/opt/mxe/usr/bin:$PATH && cd build-mxe && cpack -G NSIS64"
            }
            post {
                always {
                    archiveArtifacts artifacts: 'build-mxe/*.exe', fingerprint: true
                }
            }

        }
        stage('Debug build for Linux') {
            steps {
                sh "mkdir -p build-debug"
                sh "cd build-debug && cmake -DCMAKE_BUILD_TYPE=Debug .."
                sh "cd build-debug && make -j3"
            }
        }
        stage('Release build and Debian package for Linux') {
            steps {
                sh "mkdir -p build"
                sh "cd build && cmake -D CMAKE_BUILD_TYPE=Release .. && make -j3"
                sh "cd build && cpack -G DEB"
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

