import java.net.*;
import java.io.*;
import java.util.regex.*;

class Server {
  public static void main(String[] args) {
    if (args.length < 1) return;
    try {
      ServerSocket serverS = new ServerSocket(Integer.parseInt(args[0]));
      while (true) {
        new ServerThread(serverS.accept()).start();
        System.out.println("New connection.");
      }
    } catch  (IOException e) {
      System.out.println("IO exception.");
      System.exit(1);
    }
  }
}

class ServerThread extends Thread {
  Socket clientS;
  BufferedReader fromClient;
  PrintStream toClient;
  BufferedReader fromFile;
  String method;
  String filename;

  public ServerThread(Socket acceptedS) {
    clientS = acceptedS;
  }

  public void run() {
    try {
      fromClient = new BufferedReader(new InputStreamReader(clientS.getInputStream()));
      toClient = new PrintStream(clientS.getOutputStream(), true);

      readRequest();
      respondTo();

      fromClient.close();
      toClient.close();

      clientS.close();
    } catch (IOException e) {
      System.out.println("IO exception.");
      System.exit(1);
    }
  }

  private void readRequest() {
    try {
      Pattern p = Pattern.compile("(.+)\\s/(.+)\\sHTTP/1.1");
      Matcher m = p.matcher(fromClient.readLine());
      fromClient.readLine();
      m.find();
      method = m.group(1); filename = m.group(2);
    } catch (IOException e) {
      System.out.println("IO exception.");
      System.exit(1);
    }
  }

  private void respondTo() {
    boolean fileNotFound = false;

    try {
      fromFile = new BufferedReader(new FileReader(filename));
    } catch (IOException e) {
      fileNotFound = true;
    }

    try {
      if (fileNotFound) {
        toClient.print("HTTP/1.1 404 Not Found\r\n");
        toClient.print("Content-Type: text/html; charset=us-ascii\r\n");
        toClient.print("\r\n");
        toClient.print("<html><head><title>Not Found</title></head><body>404 Not Found</body></html>\r\n");

        return;
      }

      toClient.print("HTTP/1.1 200 OK\r\n");
      toClient.print("Content-Type: text/html; charset=us-ascii\r\n");
      toClient.print("\r\n");
      
      String line;
      while ((line = fromFile.readLine()) != null) {
        toClient.println(line);
      }

      fromFile.close();
    } catch (IOException e) {
      System.out.println("IO exception.");
      System.exit(1);
    }
  }
}

