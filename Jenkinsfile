pipeline {
  agent any
  stages {
    stage('Build') {
      steps {
        parallel(
          "Build": {
            sh 'make -d'
            sh 'make -d install'
            
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
        sh 'make publish > secure.txt'
      }
    }
    stage('Build-32bit') {
      steps {
        sh 'make -d clean'
        sh 'make -d all'
        sh 'make -d install'
      }
    }
    stage('Test-32') {
      steps {
        sh 'make test'
      }
    }
  }
}