pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                sh 'make'
                sh 'make install'
            }
            steps {
                sh 'make style'
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
