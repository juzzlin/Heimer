pipeline {
    agent any
    stages {
        stage('Build and run tests') {
            matrix {
                agent {
                    docker {
                        image 'juzzlin/qt5-20.04:latest'
                        args '--privileged -t -v $WORKSPACE:/heimer'
                    }
                }
                axes {
                    axis {
                        name 'BUILD_TYPE'
                        values 'Debug', 'Release'    
                    }
                }
                stages {
                    stage('Build and test') {
                        steps {
                            sh "mkdir -p build-$BUILD_TYPE"
                            sh "cd build-$BUILD_TYPE && cmake -GNinja -DCMAKE_BUILD_TYPE=$BUILD_TYPE .."
                            sh "cd build-$BUILD_TYPE && cmake --build . && ctest"
                        }
                    }
                }
            }
        }
        stage('Debian package / Ubuntu 20.04') {
            agent {
                docker {
                    image 'juzzlin/qt5-20.04:latest'
                    args '--privileged -t -v $WORKSPACE:/heimer'
                }
            }
            steps {
                sh "mkdir -p build-deb-ubuntu-20.04"
                sh "cd build-deb-ubuntu-20.04 && cmake -GNinja -DDISTRO_VERSION=ubuntu-20.04 -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DPACKAGE_TYPE=Deb .. && cmake --build ."
                sh "cd build-deb-ubuntu-20.04 && cpack -G DEB"
            }
            post {
                always {
                    archiveArtifacts artifacts: 'build-deb-ubuntu-20.04/*.deb', fingerprint: true
                }
            }
        }
        stage('Debian package / Ubuntu 22.04 / Qt 5') {
            agent {
                docker {
                    image 'juzzlin/qt6-22.04:latest'
                    args '--privileged -t -v $WORKSPACE:/heimer'
                }
            }
            steps {
                sh "mkdir -p build-deb-ubuntu-22.04-qt5"
                sh "cd build-deb-ubuntu-22.04-qt5 && cmake -GNinja -DDISTRO_VERSION=ubuntu-22.04 -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DPACKAGE_TYPE=Deb .. && cmake --build ."
                sh "cd build-deb-ubuntu-22.04-qt5 && cpack -G DEB"
            }
            post {
                always {
                    archiveArtifacts artifacts: 'build-deb-ubuntu-22.04-qt5/*.deb', fingerprint: true
                }
            }
        }
        stage('Debian package / Ubuntu / Qt 6') {
            matrix {
                axes {
                    axis {
                        name 'UBUNTU_VERSION'
                        values '22.04', '24.04'
                    }
                }
                stages {
                    stage('Build and run CPack') {
                        steps {
                            script {
                                def imageName = "juzzlin/qt6-${UBUNTU_VERSION}:latest"
                                docker.image(imageName).inside('--privileged -t -v $WORKSPACE:/heimer') {
                                    sh "mkdir -p build-deb-ubuntu-${UBUNTU_VERSION}-qt6"
                                    sh "cd build-deb-ubuntu-${UBUNTU_VERSION}-qt6 && cmake -GNinja -DDISTRO_VERSION=ubuntu-${UBUNTU_VERSION} -DCMAKE_BUILD_TYPE=Release -DBUILD_WITH_QT6=ON -DBUILD_TESTS=OFF -DPACKAGE_TYPE=Deb .. && cmake --build ."
                                    sh "cd build-deb-ubuntu-${UBUNTU_VERSION}-qt6 && cpack -G DEB"
                                }
                            }
                        }
                    }
                }
                post {
                    always {
                        archiveArtifacts artifacts: "build-deb-ubuntu-${UBUNTU_VERSION}-qt6/*.deb", fingerprint: true
                    }
                }
            }
        }
        stage('Windows NSIS installer') {
            agent {
                docker {
                    image 'juzzlin/mxe-qt5-20.04:latest'
                    args '-t -v $WORKSPACE:/heimer'
                }
            }
            steps {
                sh "./scripts/build-windows-nsis"
            }
            post {
                always {
                    archiveArtifacts artifacts: 'build-windows-nsis/*.exe', fingerprint: true
                }
            }
        }
        stage('Windows ZIP') {
            agent {
                docker {
                    image 'juzzlin/mxe-qt5-20.04:latest'
                    args '-t -v $WORKSPACE:/heimer'
                }
            }
            steps {
                sh "./scripts/build-windows-zip"
            }
            post {
                always {
                    archiveArtifacts artifacts: 'build-windows-zip/zip/*.zip', fingerprint: true
                }
            }
        }
        stage('AppImage') {
            agent any
            steps {
                sh "./scripts/build-app-image"
            }
            post {
                always {
                    archiveArtifacts artifacts: 'build-appimage/*.AppImage', fingerprint: true
                }
            }
        }
        stage('Source .tar.gz') {
            agent {
                docker {
                    image 'juzzlin/qt5-22.04:latest'
                    args '--privileged -t -v $WORKSPACE:/heimer'
                }
            }
            steps {
                sh "./scripts/build-archive"
            }
            post {
                always {
                    archiveArtifacts artifacts: 'heimer*.tar.gz', fingerprint: true
                }
            }
        }
    }
}
