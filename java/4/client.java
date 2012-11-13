import java.net.*;
import java.io.*;
import java.util.*;
import java.util.regex.*;

class Client {
  public static void main(String[] args) {
    try {
      BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
      Browse browser = new Browse();

      while (true) {
        System.out.print("> ");
        String url = in.readLine();

        int linkNum = 0;

        try {
          linkNum = Integer.parseInt(url);
        } catch (NumberFormatException e) {
        }

        if (linkNum != 0) {
          browser.accessLink(linkNum);
        } else {
          browser.accessURL(url);
        }
      }
    } catch (IOException e) {
      System.out.println("IO exception.");
      System.exit(1);
    }
  }
}

class Browse {
  List<String> linkList;
  String hostname, host, filePath, fileDir, fileName, pageContent;
  int port;

  public Browse() {
    linkList = new ArrayList<String>();
    pageContent = "";
  }

  public void accessURL(String url) {
    String line;

    try {
      parseURL(url);

      Socket sock = new Socket(host, port);
      BufferedReader fromServer = new BufferedReader(new InputStreamReader(sock.getInputStream()));
      PrintStream toServer = new PrintStream(sock.getOutputStream(), true);
      pageContent = "";

      toServer.print("GET " + filePath + " HTTP/1.1\r\n");
      toServer.print("Host: " + host + "\r\n\r\n");

      // skip response header
      while((line = fromServer.readLine()) != null) {
        if (line.length() == 0) {
          break;
        }
      }

      while ((line = fromServer.readLine()) != null) {
        System.out.println(line);
        pageContent += line + "\n";
      }

      fromServer.close();
      toServer.close();
      sock.close();

      updateLinkList();
      showLinkList();
    } catch (UnknownHostException e) {
      System.out.println("Unknown host.");
      System.exit(1);
    } catch (IOException e) {
      System.out.println("IO exception.");
      System.exit(1);
    }
  }


  public void accessLink(int linkNum) {
    if (linkNum <= 0 || linkNum > linkList.size()) {
      System.out.println("link Number: " + linkNum + " does not exist.");
      return;
    }
    
    accessURL(linkList.get(linkNum - 1));
  }

  private void parseURL(String url) {
    Pattern p = Pattern.compile("^http://((.+?)(:(\\d+))?)((/(.+/)*)(.+)?)$");
    Matcher m = p.matcher(url);
    m.find();

    hostname = m.group(1); host = m.group(2);
    if (m.group(4) == null) {
      port = 80;
    } else {
      port = Integer.parseInt(m.group(4));
    }

    fileName = m.group(8);
    if (m.group(8) == null) {
      fileName = "index.html";
    }
    fileDir = m.group(6);           // "/hoge/"
    filePath = fileDir + fileName;  // "/hoge/index.html"
  }

  private void updateLinkList() {
    linkList.clear();

    Pattern linkP = Pattern.compile("<a href=\"(.+?)\"");
    Matcher linkM = linkP.matcher(pageContent);

    while (linkM.find()) {
      String link = linkM.group(1);

      Pattern urlP = Pattern.compile("^http://");
      Matcher urlM = urlP.matcher(link);
      if (!urlM.find()) {
        link = "http://" + hostname + fileDir + link;
      }

      linkList.add(link);
    }
  }

  private void showLinkList() {
    for (int i = 0; i < linkList.size(); i++) {
      System.out.println(Integer.toString(i+1) + ". " + linkList.get(i));
    }
  }
}
