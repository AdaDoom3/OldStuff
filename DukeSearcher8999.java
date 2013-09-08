import java.util.*;
import java.io.*;
import java.nio.file.*;
import java.nio.file.attribute.*;
import javax.swing.*;
import javax.swing.border.*;
import java.awt.*;
import java.awt.event.*;
public class DukeSearcher8999 extends JFrame{
  private static final Dimension WINDOW_SIZE_MINIMUM   = new Dimension(600, 400);
  private static final Dimension WINDOW_SIZE_INITIAL   = new Dimension(1000, 700);
  private static final String    WINDOW_TEXT           = "MyDukeSearcher8999";
  private static final String    EXCLUDED_NAME         = "exclude_words.csv";
  private static final String    SEARCH_WORD_FAILURE   = " is in the " + EXCLUDED_NAME + " to exclude from results";
  private static final String    SEARCH_WORD_AND       = "and";
  private static final String    SEARCH_WORD_OR        = "or";
  private static final String    SEARCH_TEXT_BUTTON    = "Search";
  private static final String    SEARCH_TEXT_FAILURE   = " was not found in the searched files.";
  private static final String    SEARCH_FILE_EXTENSION = "java";
  private static final String    SEARCH_LINE_SPLIT     = "\\s|[\\]\\[!\"#$%&'()*+,./:;<=>?@\\^_`{|}~-]";
  private static final String    RESULT_PREFIX_LINE    = "Line: ";
  private static final String    RESULT_PREFIX_FILE    = "File: ";
  private static final String    RESULT_SPACING        = "   ";
  public class Result{
    public String fileName   = "";
    public int    lineNumber = 0;
    public Result(String fileName, int lineNumber){
      this.fileName   = fileName;
      this.lineNumber = lineNumber;}
    public String toString(){
      return
        RESULT_PREFIX_FILE + fileName +
        RESULT_SPACING + RESULT_PREFIX_LINE + lineNumber;}}
  public class FileSearcher{
    public class FileWalker extends SimpleFileVisitor<Path>{
      private String extension = "";
      private ArrayList<String> results = new ArrayList<String>();
      public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) throws IOException{
        if(file.toString().endsWith("." + extension))
          results.add(file.toString());
        return FileVisitResult.CONTINUE;}
      public ArrayList<String> walk(String startLocation, String extension){
        this.extension = extension;
        results.clear();
        Path start = FileSystems.getDefault().getPath(startLocation);
        try{
          Files.walkFileTree(start, this);}
        catch(IOException error){}
        return results;}} 
    private HashMap<String, ArrayList<Result>> map = new HashMap<String, ArrayList<Result>>();
    private HashMap<String, Integer> excludeMap = new HashMap<String, Integer>();
    public FileSearcher(String path, String extension){
      ArrayList<String> results = new FileWalker().walk(path, extension);
      ArrayList<Result> current;
      Scanner io;
      try{
        io = new Scanner(new BufferedReader(new FileReader(EXCLUDED_NAME)));
        for(int i = 1;io.hasNextLine();i++)
          excludeMap.put(io.nextLine().toLowerCase().split(",")[0].trim(), 1);}
      catch(IOException error){}
      String[] nextLine;
      for(String item : results){
        try{
          io = new Scanner(new BufferedReader(new FileReader(item)));
          for(int i = 1;io.hasNextLine();i++){
            nextLine = io.nextLine().toLowerCase().split(SEARCH_LINE_SPLIT);
            for(String token : nextLine)
              if(!excludeMap.containsKey(token)){
                if(map.containsKey(token))
                  current = map.get(token);
                else
                  current = new ArrayList<Result>();
                current.add(new Result(item, i));
                map.put(token, current);}}}
        catch(IOException error){}}}
    public ArrayList<Result> getResult(String key) throws Exception{
      if(excludeMap.containsKey(key))
        throw new Exception(key + SEARCH_WORD_FAILURE);
      if(!map.containsKey(key))
        throw new Exception(key + SEARCH_TEXT_FAILURE);
      ArrayList<Result> results = new ArrayList<Result>();
      for(Result item : map.get(key))
        results.add(item);
      return results;}}
  private String       lastSearchDirectory = "";
  private FileSearcher searcher;
  public DukeSearcher8999(){
    super(WINDOW_TEXT);
    final JList<String> resultList    = new JList<String>();
    final JPanel        searchPanel   = new JPanel();
    final JTextField    searchField   = new JTextField();
    final JFileChooser  browse        = new JFileChooser(".");
    final JButton       searchButton  = new JButton(SEARCH_TEXT_BUTTON);
    final Border        loweredBorder = BorderFactory.createLoweredBevelBorder();
    class SearchButtonListener implements ActionListener{
      public void actionPerformed(ActionEvent event){
        String[]          listText;
        ArrayList<Result> resultsA;
        ArrayList<Result> resultsB;
        int               mode = 0;
        if(!lastSearchDirectory.equals(browse.getCurrentDirectory().toString())){
          searcher            = new FileSearcher(browse.getCurrentDirectory().toString(), SEARCH_FILE_EXTENSION);
          lastSearchDirectory = browse.getCurrentDirectory().toString();}
        try{
          if(searchField.getText().toLowerCase().split("\\s").length == 3)
            if(searchField.getText().toLowerCase().split("\\s")[1].equals(SEARCH_WORD_AND))
              mode = 1;
            else if(searchField.getText().toLowerCase().split("\\s")[1].equals(SEARCH_WORD_OR))
              mode = 2;
          else if(searchField.getText().toLowerCase().split("\\s").length > 3)
            throw new Exception("Too many search words");
          switch(mode){
            case 0:
              resultsA = searcher.getResult(searchField.getText().toLowerCase().trim());
              listText = new String[resultsA.size()];
              for(int t = 0;t < resultsA.size();t++)
                listText[t] = resultsA.get(t).toString();
              break;
            case 1:
              resultsA = searcher.getResult(searchField.getText().toLowerCase().split("\\s")[0]);
              resultsB = searcher.getResult(searchField.getText().toLowerCase().split("\\s")[2]);
              listText = new String[resultsA.size() + resultsB.size()];
              String current = "";
              int indexA = 0;
              int indexB = 0;
              int p = 0;
              while(indexA < resultsA.size() && indexB < resultsB.size()){
                current = resultsA.get(indexA).fileName;
                if(current.equals(resultsB.get(indexB).fileName)){
                  while(indexA < resultsA.size() && resultsA.get(indexA).fileName.equals(current)){
                    listText[p++] = resultsA.get(indexA).toString() + "  " + searchField.getText().toLowerCase().split("\\s")[0];
                    indexA++;}
                  while(indexB < resultsB.size() && resultsB.get(indexB).fileName.equals(current)){
                    listText[p++] = resultsB.get(indexB).toString() + "  " + searchField.getText().toLowerCase().split("\\s")[2];;
                    indexB++;}}
                else{
                  int t = indexA;
                  for(;!current.equals(resultsA.get(t)) && t < resultsA.size();t++);
                  if(resultsA.get(t).equals(resultsB.get(indexB).fileName))
                    indexA = t;
                  else{
                    t = indexB;
                    for(;!current.equals(resultsB.get(t)) && t < resultsB.size();t++);
                    if(resultsB.get(t).equals(resultsA.get(indexA).fileName))
                      indexB = t;}}}
              break;
            case 2:
              resultsA = searcher.getResult(searchField.getText().toLowerCase().split("\\s")[0]);
              resultsB = searcher.getResult(searchField.getText().toLowerCase().split("\\s")[2]);
              listText = new String[resultsA.size() + resultsB.size() + 2];
              listText[0] = searchField.getText().toLowerCase().split("\\s")[0];
              listText[resultsA.size() + 1] = searchField.getText().toLowerCase().split("\\s")[2];
              int t = 1;
              for(t = 1;t < resultsA.size();t++)
                listText[t] = resultsA.get(t).toString();
              t = resultsA.size() + 2;
              for(int x = 0;x < resultsB.size();x++){
                listText[t] = resultsB.get(x).toString();
                t++;}
              break;
            default:
              listText = new String[]{"Error"};}}
        catch(Exception error){
          listText = new String[]{error.getMessage()};}
        resultList.setListData(listText);}} 
    setVisible(true);
    setLayout(new BorderLayout());
    setSize(       WINDOW_SIZE_INITIAL);
    setMinimumSize(WINDOW_SIZE_MINIMUM);
    setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    add(browse,                      BorderLayout.WEST);
    add(searchPanel,                 BorderLayout.PAGE_START);
    add(new JScrollPane(resultList), BorderLayout.CENTER);
    resultList.setBorder(loweredBorder);
    searchPanel.setLayout(new BoxLayout(searchPanel, BoxLayout.X_AXIS));
    searchPanel.add(searchButton);
    searchPanel.add(searchField);
    searchButton.addActionListener(new SearchButtonListener());
    browse.setBorder(loweredBorder);
    browse.setControlButtonsAreShown(false);
    browse.remove(browse.getComponents()[3]);}
  public static void main (String[] arguments){
    DukeSearcher8999 searcher = new DukeSearcher8999();}}
