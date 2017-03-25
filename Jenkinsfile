pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                sh 'make'
                sh 'make install'
            }
        }
        stage('Test') {
            steps {
                sh 'make test'
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
            }
        }
    }
}
