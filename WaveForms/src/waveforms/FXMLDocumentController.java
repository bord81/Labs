package waveforms;

import java.net.URL;
import java.util.ResourceBundle;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.XYChart;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;

public class FXMLDocumentController implements Initializable {

    private final String resultText = "vR =";
    private StringBuilder sb;

    @FXML
    private LineChart<Number, Number> chart;

    @FXML
    private TextField coefV1;

    @FXML
    private TextField coefV2;

    @FXML
    private TextField pi1;

    @FXML
    private TextField pi2;

    @FXML
    private Label lResult;

    @FXML
    private void handleButtonAction(ActionEvent event) {
        sb = new StringBuilder(resultText);
        String coefV1_str = coefV1.getText();
        String coefV2_str = coefV2.getText();
        String pi1_str = pi1.getText();
        String pi2_str = pi2.getText();
        if (coefV1_str.length() == 0 || coefV2_str.length() == 0
                || pi1_str.length() == 0 || pi2_str.length() == 0) {
            sb.append(" fill all input fields!");
            lResult.setText(sb.toString());
        } else {
            double coefV1_dbl, coefV2_dbl, pi1_dbl, pi2_dbl;
            try {
                coefV1_dbl = Double.parseDouble(coefV1_str);
                coefV2_dbl = Double.parseDouble(coefV2_str);
                pi1_dbl = Double.parseDouble(pi1_str);
                pi2_dbl = Double.parseDouble(pi2_str);
            } catch (NumberFormatException e) {
                sb.append(" input fields must contain integers only!");
                lResult.setText(sb.toString());
                return;
            }
            plot_chart(coefV1_dbl, coefV2_dbl, pi1_dbl, pi2_dbl);
        }
    }

    private void plot_chart(double cf1, double cf2, double p1, double p2) {
        double rad1, rad2, radR, rad_cycle1, rad_cycle2, rad_cycleR, cfR, angleR;
        chart.getData().clear();
        XYChart.Series<Number, Number> series_v1 = new XYChart.Series<>();
        XYChart.Series<Number, Number> series_v2 = new XYChart.Series<>();
        XYChart.Series<Number, Number> series_vr = new XYChart.Series<>();
        series_v1.setName("v1");
        series_v2.setName("v2");
        series_vr.setName("vR");
        if (p1 == 0 && p2 == 0) {
            angleR = Math.toRadians(0.0);
        } else if (p1 == 0) {
            angleR = Math.toRadians(180 - (180 / p2));
        } else if (p2 == 0) {
            angleR = Math.toRadians(180 - (180 / p1));
        } else {
            angleR = p1 > p2 ? Math.toRadians(180 - ((180 / p1) - (180 / p2))) : Math.toRadians(180 - ((180 / p2) - (180 / p1)));
        }
        cfR = Math.sqrt(Math.pow(cf1, 2) + Math.pow(cf2, 2) - (2 * cf1 * cf2 * Math.cos(angleR)));
        rad1 = p1 != 0.0 ? Math.toRadians(180 / p1) : 0.0;
        rad2 = p2 != 0.0 ? Math.toRadians(180 / p2) : 0.0;
        radR = Math.atan((cf1 * Math.sin(rad1) + cf2 * Math.sin(rad2)) / (cf1 * Math.cos(rad1) + cf2 * Math.cos(rad2)));
        if ((Math.pow(cf1, 2) + Math.pow(cfR, 2)) < Math.pow(cf2, 2)) {
            radR = Math.toRadians(180 + Math.toDegrees(radR));
        }
        for (int i = 0; i < 720; i++) {
            rad_cycle1 = rad1 + Math.toRadians(i);
            rad_cycle2 = rad2 + Math.toRadians(i);
            rad_cycleR = radR + Math.toRadians(i);
            series_v1.getData().add(new XYChart.Data<>(i, cf1 * Math.sin(rad_cycle1)));
            series_v2.getData().add(new XYChart.Data<>(i, cf2 * Math.sin(rad_cycle2)));
            series_vr.getData().add(new XYChart.Data<>(i, cfR * Math.sin(rad_cycleR)));
        }
        String sr = radR < 0 ? "sin(wt" : "sin(wt+";
        String res = " " + String.format("%.2f", cfR) + sr + String.format("%.2f", radR) + ")";
        sb.append(res);
        lResult.setText(sb.toString());
        chart.getData().add(series_v1);
        chart.getData().add(series_v2);
        chart.getData().add(series_vr);
    }

    @Override
    public void initialize(URL url, ResourceBundle rb) {
        lResult.setText(resultText);
    }

}
