pipeline {
  agent any
  stages {
    stage('Build') {
      steps {
        parallel(
          "Build": {
            sh 'make'
            sh 'make install'
            
          },
          "Style": {
            sh 'make style'
            
          }
        )
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