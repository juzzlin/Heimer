pipeline {
    agent any
    stages {
        stage('CMake Debug build for Linux') {
            steps {
                sh "mkdir -p build-debug"
                sh "cd build-debug && cmake -DCMAKE_BUILD_TYPE=Debug .."
                sh "cd build-debug && cmake --build . --target all -- -j3"
            }
        }
        stage('Run tests') {
            steps {
                sh "cd build-debug && ctest"
            }
        }
        stage('QMake build for Linux') {
            steps {
                sh "mkdir -p build-qmake"
                sh "cd build-qmake && qmake .. && make -j3"
            }
        }
        stage('Debian package for Linux') {
            steps {
                sh "mkdir -p build"
                sh "cd build && cmake -D CMAKE_BUILD_TYPE=Release .. && cmake --build . --target all -- -j3"
                sh "cd build && cpack -G DEB"
            }
        }
        stage('NSIS installer for Windows') {
            agent {
                docker {
                    image 'juzzlin/mxe-qt5:latest'
                    args '--privileged -t -v $WORKSPACE:/heimer'
                }
            }
            steps {
                sh "./scripts/buildWindowsInstaller.sh"
            }
            post {
                always {
                    archiveArtifacts artifacts: 'build-windows-docker/release/*.exe', fingerprint: true
                }
            }

        }
    }
    post {
        always {
            archiveArtifacts artifacts: 'build/*.deb', fingerprint: true
        }
    }
}

