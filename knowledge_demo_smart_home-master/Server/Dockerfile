FROM openjdk:8-jre
WORKDIR /app
ADD distschedule-core/target/distschedule-core-1.0.0-SNAPSHOT.jar distschedule-core-1.0.0-SNAPSHOT.jar
EXPOSE 8080
ENTRYPOINT ["java","-jar"]
CMD ["distschedule-core-1.0.0-SNAPSHOT.jar"]
# docker build -t distschedule .