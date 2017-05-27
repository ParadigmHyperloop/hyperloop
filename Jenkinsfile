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
        parallel(
          "Deploy": {
            sh 'make deb'
            sh 'make publish'
            
          },
          "Archive": {
            archiveArtifacts './BUILD/dst'
            
          }
        )
      }
    }
    stage('Build-32bit') {
      steps {
        sh 'make clean'
        sh 'make all'
        sh 'make install'
      }
    }
    stage('Test-32') {
      steps {
        sh 'make test'
      }
    }
  }
}