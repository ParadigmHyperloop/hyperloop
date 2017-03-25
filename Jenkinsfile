pipeline {
    agent any

    stages {
        stage('Build') {
            parallel {
                "build" : {
                    steps {
                        sh 'make'
                        sh 'make install'
                    }
                }
                "style" : {
                    steps {
                        sh 'make style'
                    }
                }
            }
        }
        stage('Test') {
            steps {
                sh 'make test'
            }
        }
        stage('Deploy') {
            steps {
                sh 'make deb'
                sh 'make publish'
            }
        }
    }
}
