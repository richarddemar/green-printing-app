package GreenPrinting_User;


import java.awt.Color;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.Toolkit;

import javax.swing.JFrame;
import javax.swing.JLabel;


import javax.swing.JButton;
import javax.swing.JPanel;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.io.*;

import javax.swing.SwingConstants;

import java.awt.Font;

import javax.swing.ImageIcon;
import javax.swing.border.BevelBorder;

public class User {

	
	private static int pageNum;
	
	private String warningText = String.format("<html><div>Users are responsible for paying for all their printed pages. "
			+ "Please see a staff person for assistance with printing.</div><html>");

	private double price = 0.0;
	private String priceText = "$ " + price;
	private double cost = 0.0;


	private JFrame frame;
	JButton okay_btn;
	JButton cancel_btn;
	JLabel warningTextLb;
	private JLabel priceTextLb;
	private JLabel costTextLb;
	private JLabel notificationImg;
	private JLabel pageNumTextLb;
	private JPanel warningTextLbPane;
	private JPanel pricePane;
	private JLabel priceLb;
	private JPanel pagePane;
	private JLabel pageNumLb;
	private JPanel CostPane;
	private JLabel costLb;

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		String arg = args[0];
		if(arg.matches("/pagecount:(.*)")) {
			pageNum = Integer.parseInt(arg.substring("/pagecount:".length()));
			//System.out.println(pageNum);
		}
		
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					User window = new User();
					window.frame.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
				
			}
		});
		
	}

	/**
	 * Create the application.
	 */
	public User() {
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		
		setWarningPriceText();
		setCost();

		frame = new JFrame();
		frame.getContentPane().setBackground(new Color(95, 158, 160));
		frame.setBounds(100, 100, 550, 300);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		Dimension dim = Toolkit.getDefaultToolkit().getScreenSize();
		frame.setLocation(dim.width/2-frame.getSize().width/2, dim.height/2-frame.getSize().height/2);
		frame.setUndecorated(true);
		//frame.getRootPane().setBorder(BorderFactory.createLineBorder(Color.DARK_GRAY));
		//setWindowDecorationStyle(JRootPane.NONE);
		
		
		okay_btn = new JButton("OK");
		okay_btn.setFont(new Font("Arial", Font.PLAIN, 14));
		okay_btn.setToolTipText("Click \"OK\" to start printing.");
		okay_btn.setBounds(167, 249, 90, 40);
		okay_btn.setFocusPainted(false);
		okay_btn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				okay_btnActionPerformed(evt);
			}
		});
		frame.getContentPane().setLayout(null);
		frame.getContentPane().add(okay_btn);
		
		cancel_btn = new JButton("Cancel");
		cancel_btn.setFont(new Font("Arial", Font.PLAIN, 14));
		cancel_btn.setToolTipText("Click \"Cancel\" to stop current printing job.");
		cancel_btn.setBounds(324, 249, 90, 40);
		cancel_btn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				cancel_btnActionPerformed(evt);
			}
		});
		frame.getContentPane().add(cancel_btn);

		notificationImg = new JLabel("");
		notificationImg.setHorizontalAlignment(SwingConstants.CENTER);
		notificationImg.setIcon(new ImageIcon(User.class.getResource("notification_img.png")));
		notificationImg.setBounds(0, 40, 60, 92);
		frame.getContentPane().add(notificationImg);
		
		pricePane = new JPanel();
		pricePane.setBorder(new BevelBorder(BevelBorder.LOWERED, null, null, null, null));
		pricePane.setBounds(59, 143, 150, 80);
		frame.getContentPane().add(pricePane);
		
		priceTextLb = new JLabel("Price per page:");
		priceTextLb.setBounds(10, 15, 107, 17);
		priceTextLb.setHorizontalAlignment(SwingConstants.CENTER);
		priceTextLb.setFont(new Font("Arial", Font.BOLD, 14));
		pricePane.setLayout(null);
		
		priceLb = new JLabel(priceText);
		priceLb.setLocation(10, 43);
		priceLb.setFont(new Font("Arial", Font.BOLD, 20));
		priceLb.setHorizontalAlignment(SwingConstants.CENTER);
		priceLb.setSize(priceLb.getPreferredSize());
		pricePane.add(priceLb);
		pricePane.add(priceTextLb);
		
		pagePane = new JPanel();
		pagePane.setBorder(new BevelBorder(BevelBorder.LOWERED, null, null, null, null));
		pagePane.setBounds(216, 143, 150, 80);
		frame.getContentPane().add(pagePane);
		
		pageNumTextLb = new JLabel("Number of pages:");
		pageNumTextLb.setBounds(10, 15, 124, 17);
		pageNumTextLb.setFont(new Font("Arial", Font.BOLD, 14));
		pagePane.setLayout(null);
		
		pageNumLb = new JLabel();
		pageNumLb.setText("" + pageNum);
		pageNumLb.setLocation(10, 43);
		pageNumLb.setFont(new Font("Arial", Font.BOLD, 20));
		pageNumLb.setSize(pageNumLb.getPreferredSize());
		pagePane.add(pageNumLb);
		pagePane.add(pageNumTextLb);
		
		CostPane = new JPanel();
		CostPane.setBorder(new BevelBorder(BevelBorder.LOWERED, null, null, null, null));
		CostPane.setBounds(372, 143, 150, 80);
		frame.getContentPane().add(CostPane);
		
		costTextLb = new JLabel("Total costs:");
		costTextLb.setBounds(10, 15, 80, 17);
		costTextLb.setFont(new Font("Arial", Font.BOLD, 14));
		
		costLb = new JLabel("$ " + cost);
		costLb.setLocation(10, 43);
		costLb.setFont(new Font("Arial", Font.BOLD, 20));
		costLb.setSize(costLb.getPreferredSize());
		CostPane.setLayout(null);
		CostPane.add(costTextLb);
		CostPane.add(costLb);
		
		warningTextLbPane = new JPanel();
		warningTextLbPane.setBorder(new BevelBorder(BevelBorder.LOWERED, null, null, null, null));
		warningTextLbPane.setBounds(59, 40, 463, 92);
		frame.getContentPane().add(warningTextLbPane);
		warningTextLbPane.setLayout(null);
		
		warningTextLb = new JLabel(warningText);
		warningTextLb.setBounds(10, 2, 451, 88);
		warningTextLbPane.add(warningTextLb);
		warningTextLb.setFont(new Font("Arial", Font.ITALIC, 17));
		warningTextLb.setHorizontalAlignment(SwingConstants.CENTER);
		
	}
	
	private void okay_btnActionPerformed(ActionEvent evt) {
		if (evt.getSource() == okay_btn) {
			System.exit(1);
		}
	}
	
	private void cancel_btnActionPerformed(ActionEvent evt) {
		if (evt.getSource() == cancel_btn) {
			System.exit(0);
		}
	}
	
	String readFile(String fileName) throws IOException {
	    BufferedReader br = new BufferedReader(new FileReader(fileName));
	    try {
	        StringBuilder sb = new StringBuilder();
	        String line = br.readLine();

	        while (line != null) {
	            sb.append(line);
	            sb.append(" ");
	            line = br.readLine();
	        }
	        return sb.toString();
	    } finally {
	        br.close();
	    }
	}

	private void setWarningPriceText() {
		try {
			String root = System.getProperty("user.dir");
			String warningTextFilepath = "/rsc/warningText.txt";
			String warningTextAbspath = root+warningTextFilepath;
			this.warningText = String.format("<html><div>%s</div><html>",readFile(warningTextAbspath));
			
			String priceTextFilepath = "/rsc/price.txt";
			String priceTextAbspath = root+priceTextFilepath;
			this.priceText = readFile(priceTextAbspath);
			this.price = Double.parseDouble(this.priceText);
			
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	

	/**
	 * @param cost the cost to set
	 */
	public void setCost() {
		this.cost = Math.round(price * pageNum*100);
		this.cost = this.cost/100;
	}
	
}
