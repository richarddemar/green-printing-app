package GreenPrinting_Admin;

import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.Toolkit;

import javax.swing.AbstractAction;
import javax.swing.ActionMap;
import javax.swing.InputMap;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.KeyStroke;
import javax.swing.SpinnerModel;
import javax.swing.SpinnerNumberModel;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.io.*;

import javax.swing.SwingConstants;

import java.awt.Font;

import javax.swing.border.BevelBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.event.UndoableEditEvent;
import javax.swing.event.UndoableEditListener;
import javax.swing.text.Document;
import javax.swing.undo.CannotUndoException;
import javax.swing.undo.UndoManager;
import javax.swing.JTextArea;
import javax.swing.JSpinner;
import javax.swing.UIManager;

public class Admin {

	
	
	private String warningText;

	private String price = "0.0";
	
	private JFrame frame;
	JButton okay_btn;
	JButton cancel_btn;
	private JPanel warningTextPane;
	private JTextArea textArea;
	private JLabel setWarningLb;
	private JButton textClearBtn;
	private JPanel pricePane;
	private JLabel setPriceLb;
	private JSpinner spinner1;
	private JSpinner spinner2;
	private JSpinner spinner3;
	private JLabel dollarSignLb;
	private JLabel decPointLb;
	private JButton priceClearBtn;
	private JButton applyBtn;

	private UndoManager undoManager;
	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {

		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					Admin window = new Admin();
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
	public Admin() {
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
	
		frame = new JFrame();
		frame.setBounds(100, 100, 550, 410);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		Dimension dim = Toolkit.getDefaultToolkit().getScreenSize();
		frame.setLocation(dim.width/2-frame.getSize().width/2, dim.height/2-frame.getSize().height/2);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		okay_btn = new JButton("OK");
		okay_btn.setFont(new Font("Arial", Font.PLAIN, 14));
		okay_btn.setToolTipText("Click \"OK\" to save and quit.");
		okay_btn.setBounds(225, 330, 90, 30);
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
		cancel_btn.setToolTipText("Click \"Cancel\" to quit.");
		cancel_btn.setBounds(325, 330, 90, 30);
		cancel_btn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				cancel_btnActionPerformed(evt);
			}
		});
		frame.getContentPane().add(cancel_btn);
		
		warningTextPane = new JPanel();
		warningTextPane.setBorder(UIManager.getBorder("TextField.border"));
		warningTextPane.setBounds(10, 11, 514, 158);
		frame.getContentPane().add(warningTextPane);
		warningTextPane.setLayout(null);
		
		textArea = new JTextArea();
		textArea.setToolTipText("Press \"Ctrl+z\" to undo changes.");
		textArea.setLocation(10, 33);
		textArea.setSize(490, 73);
		textArea.setLineWrap(true);
		setWarningText();
		textArea.setBorder(new BevelBorder(BevelBorder.LOWERED, null, null, null, null));
		textArea.getDocument().addDocumentListener(new DocumentListener() {
			 
		    public void insertUpdate(DocumentEvent e) {
		    	textClearBtn.setEnabled(true);
		    	applyBtn.setEnabled(true);
		    }
		    public void removeUpdate(DocumentEvent e) {
		    	textClearBtn.setEnabled(true);
		    	applyBtn.setEnabled(true);
		    }
		    public void changedUpdate(DocumentEvent e) {
		    	textClearBtn.setEnabled(true);
		    	applyBtn.setEnabled(true);
		    }
		   
		});
		warningTextPane.add(textArea);
		
		undoManager = new UndoManager();
		Document doc = textArea.getDocument();
		doc.addUndoableEditListener(new UndoableEditListener() {
		    @Override
		    public void undoableEditHappened(UndoableEditEvent e) {
		        //System.out.println("Add edit");
		        undoManager.addEdit(e.getEdit());

		    }
		});

		InputMap im = textArea.getInputMap(JComponent.WHEN_FOCUSED);
		ActionMap am = textArea.getActionMap();

		im.put(KeyStroke.getKeyStroke(KeyEvent.VK_Z, Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()), "Undo");
		im.put(KeyStroke.getKeyStroke(KeyEvent.VK_Y, Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()), "Redo");

		am.put("Undo", new AbstractAction() {
		    @Override
		    public void actionPerformed(ActionEvent e) {
		        try {
		            if (undoManager.canUndo()) {
		                undoManager.undo();
		            }
		        } catch (CannotUndoException exp) {
		            exp.printStackTrace();
		        }
		    }
		});
		am.put("Redo", new AbstractAction() {
		    @Override
		    public void actionPerformed(ActionEvent e) {
		        try {
		            if (undoManager.canRedo()) {
		                undoManager.redo();
		            }
		        } catch (CannotUndoException exp) {
		            exp.printStackTrace();
		        }
		    }
		});
		
		setWarningLb = new JLabel("Set the warning in the text box below:");
		setWarningLb.setFont(new Font("Arial", Font.PLAIN, 16));
		setWarningLb.setBounds(33, 0, 284, 33);
		warningTextPane.add(setWarningLb);
		
		textClearBtn = new JButton("Clear");
		textClearBtn.setFont(new Font("Arial", Font.PLAIN, 14));
		textClearBtn.setBounds(415, 117, 89, 30);
		textClearBtn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				textClearBtnActionPerformed(evt);
			}
		});
		warningTextPane.add(textClearBtn);
		
		pricePane = new JPanel();
		pricePane.setBorder(UIManager.getBorder("TextField.border"));
		pricePane.setBounds(10, 180, 514, 106);
		frame.getContentPane().add(pricePane);
		pricePane.setLayout(null);
		
		setPriceLb = new JLabel("Set the price per page:");
		setPriceLb.setHorizontalAlignment(SwingConstants.CENTER);
		setPriceLb.setFont(new Font("Arial", Font.PLAIN, 16));
		setPriceLb.setBounds(0, 0, 225, 59);
		pricePane.add(setPriceLb);
		
		SpinnerModel spinnerModel1 = new SpinnerNumberModel(0, 0, 9, 1);
		spinner1 = new JSpinner(spinnerModel1);
		spinner1.setFont(new Font("Arial", Font.BOLD, 24));
		spinner1.setBounds(243, 6, 36, 50);
		spinner1.setBorder(new BevelBorder(BevelBorder.LOWERED, null, null, null, null));
		spinner1.addChangeListener(new SpinnerListener());
		pricePane.add(spinner1);
		
		dollarSignLb = new JLabel("$");
		dollarSignLb.setFont(new Font("Arial", Font.BOLD, 16));
		dollarSignLb.setHorizontalAlignment(SwingConstants.CENTER);
		dollarSignLb.setBounds(207, 14, 25, 35);
		pricePane.add(dollarSignLb);
		
		SpinnerModel spinnerModel2 = new SpinnerNumberModel(0, 0, 9, 1);
		spinner2 = new JSpinner(spinnerModel2);
		spinner2.setFont(new Font("Arial", Font.BOLD, 24));
		spinner2.setBounds(306, 6, 36, 50);
		spinner2.setBorder(new BevelBorder(BevelBorder.LOWERED, null, null, null, null));
		spinner2.addChangeListener(new SpinnerListener());
		pricePane.add(spinner2);
		
		SpinnerModel spinnerModel3 = new SpinnerNumberModel(0, 0, 9, 1);
		spinner3 = new JSpinner(spinnerModel3);
		spinner3.setFont(new Font("Arial", Font.BOLD, 24));
		spinner3.setBounds(342, 6, 36, 50);
		spinner3.setBorder(new BevelBorder(BevelBorder.LOWERED, null, null, null, null));
		spinner3.addChangeListener(new SpinnerListener());
		pricePane.add(spinner3);
		
		decPointLb = new JLabel(".");
		decPointLb.setHorizontalAlignment(SwingConstants.CENTER);
		decPointLb.setFont(new Font("Arial Rounded MT Bold", Font.BOLD, 26));
		decPointLb.setBounds(285, 10, 16, 41);
		pricePane.add(decPointLb);
		
		priceClearBtn = new JButton("Clear");
		priceClearBtn.setFont(new Font("Arial", Font.PLAIN, 14));
		priceClearBtn.setBounds(415, 65, 89, 30);
		priceClearBtn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				priceClearBtnActionPerformed(evt);
			}
		});
		pricePane.add(priceClearBtn);
		
		applyBtn = new JButton("Apply");
		applyBtn.setToolTipText("Click \"Apply\" to save current settings.");
		applyBtn.setBounds(425, 330, 89, 30);
		applyBtn.setEnabled(false);
		applyBtn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				applyBtnActionPerformed(evt);
			}
		});
		frame.getContentPane().add(applyBtn);
		applyBtn.setFont(new Font("Arial", Font.PLAIN, 14));
		
	}
	
	private void okay_btnActionPerformed(ActionEvent evt) {
		if (evt.getSource() == okay_btn) {
			if(applyBtn.isEnabled()) {
				apply();
			}
			System.exit(0);
		}
	}
	
	private void cancel_btnActionPerformed(ActionEvent evt) {
		if (evt.getSource() == cancel_btn) {
			System.exit(0);
		}
	}
	
	private void applyBtnActionPerformed(ActionEvent evt) {
		if (evt.getSource() == applyBtn) {
			apply();
			applyBtn.setEnabled(false);
		}
	}
	
	private void textClearBtnActionPerformed(ActionEvent evt) {
		if (evt.getSource() == textClearBtn) {
			textArea.setText("");
			textClearBtn.setEnabled(false);
			applyBtn.setEnabled(true);
		}
	}
	
	private void priceClearBtnActionPerformed(ActionEvent evt) {
		if (evt.getSource() == priceClearBtn) {
			spinner1.setValue(0);
			spinner2.setValue(0);
			spinner3.setValue(0);
			priceClearBtn.setEnabled(false);
			price = "0";
		}
	}
	
	
	class SpinnerListener implements ChangeListener {
		/**
		 * This method overrides the actionPerformed() method in the
		 * ActionListener interface
		 * 
		 * @param e
		 *            the event occurred on the spinner
		 */
		@Override
		public void stateChanged(ChangeEvent e) {
			if(e.getSource() == spinner1 || e.getSource() == spinner2 || e.getSource() == spinner3) {
				priceClearBtn.setEnabled(true);
				applyBtn.setEnabled(true);
			}
		}
		

	}
	
	private void apply() {
		price = spinner1.getValue() + "." + spinner2.getValue() + spinner3.getValue();
		warningText = textArea.getText();
		output("price", price);
		output("warningText", warningText);
	}
	
	private void output(String file, String content) {
		try {
			String root = System.getProperty("user.dir");
			String filepath = "/rsc/"+file+".txt";
			String abspath = root+filepath;
		    PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(abspath, false)));
		    out.print(content);
		    out.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	String readFile(String fileName) throws IOException {
	    BufferedReader br = new BufferedReader(new FileReader(fileName));
	    try {
	        StringBuilder sb = new StringBuilder();
	        String line = br.readLine();

	        while (line != null) {
	            sb.append(line);
	            sb.append("\n");
	            line = br.readLine();
	        }
	        return sb.toString();
	    } finally {
	        br.close();
	    }
	}
	
	private void setWarningText() {
		try {
			String root = System.getProperty("user.dir");
			String filepath = "/rsc/warningText.txt";
			String abspath = root+filepath;
			textArea.setText(readFile(abspath));
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	

}
