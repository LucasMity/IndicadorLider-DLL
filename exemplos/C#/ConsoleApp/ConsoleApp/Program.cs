
using System.Runtime.InteropServices;

namespace Lider
{
    class Program {
        enum INDICADOR_STATUS_BALANCA
        {
            STATUS_BALANCA_PESO_EXCESSO_POSITIVO = -3,
            STATUS_BALANCA_PESO_EXCESSO_NEGATIVO = -2,
            STATUS_BALANCA_NULO = -1,
            STATUS_BALANCA_ESTAVEL = 0,
            STATUS_BALANCA_INSTAVEL = 1,
            STATUS_BALANCA_VALOR_PICO = 2,
            STATUS_BALANCA_PESO_MEDIO = 3,
            STATUS_BALANCA_AGUARDANDO = 4,
            STATUS_BALANCA_PERDA_COMUNICACAO = 5,
            STATUS_BALANCA_IMPRIME = 6,
        }

        const String dllDir = "../../../../../../../lib/x64/indicadorLider.dll";
        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern int openSerial(string port, int baudRate, int dataBits, int stopBits, int parity);

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern int openTcp(string ip, int port);

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern int isOpen();

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern int close();

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern int zerar();

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern int tarar();

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern int tararManual( int tara );

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern double getPesoLiquido();

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern double getPesoBruto();

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern double getTara();

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern int getPrecisaoDecimalPeso();

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern int getStatusBalanca();

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern int getUnidadeMedida();

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern int isModoContadoraDePecas();

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern int getProtocolo();

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern int getStatusBateriaIndicador();

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern int getNivelBateriaIndicador();

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern int getNivelBateriaTransmissorTx1();

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern int getNivelBateriaTransmissorTx2();

        [DllImport(dllDir, CharSet = CharSet.Ansi)]
        public static extern IntPtr gerarRelatorio(int tipo, string input);

        static bool Check(int response)
        {
            if (response > 0)
            {
                Console.WriteLine("SUCESSO");
                return true;
            } else
            {
                Console.WriteLine("ERRO");
                return false;
            }
        }

        static void Main(string[] args)
        {
            //openSerial("COM4", 9600, 8, 1, 0);
            openTcp("10.20.0.200", 502);
            int opc = 1, tara;
            string output;
            int precisao;
            while (opc != 0)
            {
                Console.Write("1 - ZERAR\n2 - MOSTRAR VALORES\n3 - TARAR\n4 - TARAR MANUAL\n5 - GERAR RELATORIO COM AS 10K ULTIMAS IMPRESSOES\n0 - SAIR\nOPC: ");
                int.TryParse(Console.ReadLine(), out opc);
                switch (opc)
                {
                    case 0:
                        Console.WriteLine("Finalizando...");
                        break;
                    case 1:
                        Check(zerar());
                        break;
                    case 2:
                        precisao = getPrecisaoDecimalPeso();
                        if (getStatusBalanca() >= 0)
                        {
                            if (isModoContadoraDePecas() == 1)
                                Console.WriteLine($"Pecas       : {(int)getPesoLiquido()}");
                            else
                                Console.WriteLine($"Peso Liquido: " + string.Format("{0:F" + precisao + "}", getPesoLiquido()));
                            Console.WriteLine($"Peso Bruto  : " + string.Format("{0:F" + precisao + "}", getPesoBruto()));
                            Console.WriteLine($"Tara        : " + string.Format("{0:F" + precisao + "}", getTara()));
                        }

                        Console.Write("Status      : ");
                        switch(getStatusBalanca())
                        {
                            case (int)INDICADOR_STATUS_BALANCA.STATUS_BALANCA_PESO_EXCESSO_POSITIVO:
                                Console.WriteLine("Peso em Excesso Positivo");
                                break;
                            case (int)INDICADOR_STATUS_BALANCA.STATUS_BALANCA_PESO_EXCESSO_NEGATIVO:
                                Console.WriteLine("Peso em Excesso Negativo");
                                break;
                            case (int)INDICADOR_STATUS_BALANCA.STATUS_BALANCA_ESTAVEL:
                                Console.WriteLine("Peso Estavel");
                                break;
                            case (int)INDICADOR_STATUS_BALANCA.STATUS_BALANCA_INSTAVEL:
                                Console.WriteLine("Peso Instavel");
                                break;
                            case (int)INDICADOR_STATUS_BALANCA.STATUS_BALANCA_VALOR_PICO:
                                Console.WriteLine("Valor de Pico");
                                break;
                            case (int)INDICADOR_STATUS_BALANCA.STATUS_BALANCA_PESO_MEDIO:
                                Console.WriteLine("Peso Medio");
                                break;
                            case (int)INDICADOR_STATUS_BALANCA.STATUS_BALANCA_AGUARDANDO:
                                Console.WriteLine("Aguardando");
                                break;
                            case (int)INDICADOR_STATUS_BALANCA.STATUS_BALANCA_PERDA_COMUNICACAO:
                                Console.WriteLine("Perda de Comunicacao");
                                break;
                            case (int)INDICADOR_STATUS_BALANCA.STATUS_BALANCA_IMPRIME:
                                Console.WriteLine("Imprimindo");
                                break;
                            default:
                                Console.WriteLine("Sem informacoes");
                                break;
                        }

                        Console.WriteLine($"PROTOCOLO LIDER {getProtocolo() + 1}");
                        Console.Write("UNIDADE DE MEDIDA: ");
                        switch (getUnidadeMedida())
                        {
                            case 2:
                                Console.WriteLine("KG");
                                break;
                            case 1:
                                Console.WriteLine("LB");
                                break;
                            case 0:
                                Console.WriteLine("KN");
                                break;
                            default:
                                Console.WriteLine("Desconhecido");
                                break;
                        }
                        break;
                    case 3:
                        Check(tarar());
                        break;
                    case 4:
                        Console.Write("TARA: ");
                        if (int.TryParse(Console.ReadLine(), out tara))
                        {
                            Check(tararManual(tara));
                        }
                        else
                            Console.WriteLine("Valor de tara invalido");
                        break;
                    case 5:
                        output = Marshal.PtrToStringAnsi(gerarRelatorio(0, "10000"));
                        Console.WriteLine(output);
                        break;
                    default:
                        Console.WriteLine("Opcao invalida.");
                        break;
                }
            }
            close();
        }
    }
}
