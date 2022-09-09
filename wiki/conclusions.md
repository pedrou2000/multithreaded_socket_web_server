## Conclusions

### Technical Conclusions

The developed server can perfectly handle several concurrent connections, answering them correctly if the requests are well formed and
understandable to the server, executing scripts if it the case and can answer OPTIONS  requests correctly. The server can be configured with the 4 different parameters explained previously. Also the the index.html has been modified to use the newly programmed scripts, allowing the user to input data, which the scripts will process adequately.

### Personal Conclusions

In this practice we have mainly learned how a sever works internally. The fact that the server must be programmed in c made us understand deeply how
 the internal low level part of the server works, at the cost of making this practice very difficult at the beginning and very complex to implement.
  The structure of the the http messages has also been reviewed, including its fields, headers and structure of the body. We have also learned some
  html in order to change the given web page in order to include the python scripts we have programmed. Also the use of different libraries such as
  libconfuse and picohttpparser has been learned, together with the creation of static libraries created by the
programmer. Therefore this practice has taught us many new skills whereas the time we have put to it is probably excessively high.
