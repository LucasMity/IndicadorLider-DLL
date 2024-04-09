import com.sun.jna.Library;
import com.sun.jna.Native;

import java.text.DecimalFormat;
import java.util.Scanner;

public class App {

    public static boolean check(int response) {
        if (response > 0) {
            System.out.println("SUCESSO");
            return true;
        } else {
            System.out.println("ERRO");
            return false;
        }
    }

    public interface Indicador extends Library {
        public int openSerial ( String port, int baudRate, int dataBits, int stopBits, int parity );
        public int openTcp ( String ip, int port );
        public int isOpen ( );
        public int close ( );
        public int zerar ( );
        public int tarar ( );
        public int tararManual( int tara );

        public double getPesoLiquido ( );
        public double getPesoBruto ( );
        public double getTara ( );
        public int getPrecisaoDecimalPeso ( );
        public int getStatusBalanca ( );
        public int getUnidadeMedida ( );
        public int isModoContadoraDePecas ( );
        public int getProtocolo ( );
        public int getStatusBateriaIndicador ( );
        public int getNivelBateriaIndicador ( );
        public int getNivelBateriaTransmissorTx1 ( );
        public int getNivelBateriaTransmissorTx2 ( );
        public String gerarRelatorio ( int tipo, String input );
        public void freeStringMemory ( );
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.println();
        Indicador ind = (Indicador) Native.load("../../../lib/indicadorLider64.dll", Indicador.class);

        //ind.openSerial("COM4", 9600, 8, 1, 0);
        ind.openTcp("10.20.0.200", 502);
        int opc = 1;
        int tara;
        int precisaoPeso;
        String output;
        DecimalFormat df = new DecimalFormat();

        while (opc != 0) {
            System.out.print("1 - ZERAR\n" + 
                                "2 - MOSTRAR VALORES\n" + 
                                "3 - TARAR\n" + 
                                "4 - TARAR MANUAL\n" + 
                                "5 - GERAR RELATORIO COM AS 10K ULTIMAS IMPRESSOES\n" +
                                "0 - SAIR\n" + 
                                "OPC: ");
            opc = scanner.nextInt();
            switch(opc) {
                case 1:
                    check(ind.zerar());
                    break;
                case 2:
                    precisaoPeso = ind.getPrecisaoDecimalPeso();
                    df.setMinimumFractionDigits(precisaoPeso);
                    if (ind.getStatusBalanca() >= 0) {
                        if (ind.isModoContadoraDePecas() == 1)
                            System.out.println("Pecas:        " + (int)ind.getPesoLiquido() );
                        else
                            System.out.println("Peso Liquido: " + df.format(ind.getPesoLiquido()));
                        System.out.println("Peso Bruto:   " + df.format(ind.getPesoBruto()));
                        System.out.println("Tara:         " + df.format(ind.getTara()));
                    }

                    System.out.print("Status: ");
                    switch(ind.getStatusBalanca())
                    {
                        case -3:
                            System.out.println("Peso com Excesso Positivo");
                            break;
                        case -2:
                            System.out.println("Peso com Excesso Negativo");
                            break;
                        case 0:
                            System.out.println("Peso Estavel");
                            break;
                        case 1:
                            System.out.println("Peso Instavel");
                            break;
                        case 2:
                            System.out.println("Valor de Pico");
                            break;
                        case 3:
                            System.out.println("Peso Medio");
                            break;
                        case 4:
                            System.out.println("Aguardando");
                            break;
                        case 5:
                            System.out.println("Perda de Comunicacao");
                            break;
                        case 6:
                            System.out.println("Imprimindo");
                            break;
                        default:
                            System.out.println("Sem informacao");
                    }
                    
                    System.out.println("PROTOCOLO LIDER " + (ind.getProtocolo() + 1));
                    System.out.print("UNIDADE DE MEDIDA: ");
                    switch(ind.getUnidadeMedida()) {
                        case 2:
                            System.out.println("KG");
                            break;
                        case 1:
                            System.out.println("LB");
                            break;
                        case 0:
                            System.out.println("KN");
                            break;
                        default:
                            System.out.println("Desconhecido");
                    }
                    break;
                case 3:
                    check(ind.tarar());
                    break;
                case 4:
                    System.out.print("TARA: ");
                    tara = scanner.nextInt();
                    check(ind.tararManual(tara));
                    break;
                case 5:
                    output = ind.gerarRelatorio(0, "10000");
                    ind.freeStringMemory();
                    System.out.println(output);
                    break;
                case 0:
                    System.out.println("Finalizando...");
                    break;
                default:
                    break;
            }
        }
        ind.close();
        scanner.close();
    }

}