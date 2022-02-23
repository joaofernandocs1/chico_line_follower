import dash
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output, State
import time
import plotly
import random
import plotly.graph_objs as go
from collections import deque
import websocket
import json


ws = websocket.WebSocket()


# # # CSS + JS # # #

external_css = ["https://cdnjs.cloudflare.com/ajax/libs/materialize/0.100.2/css/materialize.min.css"]
external_js = ['https://cdnjs.cloudflare.com/ajax/libs/materialize/0.100.2/js/materialize.min.js']


# # # DASH # # #

app = dash.Dash('vehicle-data',
                 external_scripts=external_js,
                 external_stylesheets=external_css
               )


# # # VARIABLES # # #

signs_dict = {"erro": 0, "diff": 0}
cons_dict = {"kp": 0, "ki": 0, "kd": 0}
ord_dict = {"ordem": ""}
conf_dict = {"Rede": "", "BT": ""}
bat_dict = {"bat": 0}
vel_dict = {"vel": 0}

'''conf_dict = dict(getConf()) 
print(conf_dict)'''

max_length = 40

times = deque(maxlen=max_length)
sinal_diff = deque(maxlen=max_length)
sinal_erro = deque(maxlen=max_length)

'''para não ter que perguntar ao robô em qual estado ele está, admite-se
um estado qualquer. Em algum instante, o estado do robô coincidirá com o
declarado no programa. O que acontecerá é que talvez o usuário precise
clicar mais de uma vez para trocar o estado do robô
'''

andando = True
ordem = "stop"

end_webs = "ws://{}:80" # ws.connect("ws://192.168.0.28:80")
end_webs_comp = ""

labels = ['Consumido', 'Restante']
values = [4095 - bat_dict["bat"], bat_dict["bat"]]

# # # ATUALIZAÇÃO E TROCA DE DADOS # # #

def getCons():
    ws.send("send")
    ws.send("cons")
    recData = ws.recv()
    cons_dict = json.loads(recData)

    return cons_dict


def getConf():
    ws.send("send")
    ws.send("conf")
    recData = ws.recv()
    conf_dict = json.loads(recData)

    return conf_dict


def getSigns():
    ws.send("send")
    ws.send("signs")
    recData = ws.recv()
    signs_dict = json.loads(recData)

    return signs_dict

def getBat():
    ws.send("send")
    ws.send("bat")
    recData = ws.recv()
    bat_dict = json.loads(recData)

    return bat_dict


def senVmax(vmax):
    ws.send("rec")
    ws.send("vel")
    vel_dict["vel"] = vmax
    ws.send(json.loads(vel_dict))


def senCons(kp, ki, kd):
    ws.send("rec")
    ws.send("cons")
    cons_dict["kp"] = kp
    cons_dict["ki"] = ki
    cons_dict["kd"] = kd
    ws.send(json.dumps(cons_dict))


def senOrd(ord):
    ws.send("rec")
    ws.send("ord")
    ord_dict["ordem"] = ordem
    ws.send(json.dumps(ord_dict))

#def update_signs_values(times, sinal_diff, sinal_erro):

#    times.append(time.time() - start)

#    if ws.connected:

#        signs_dict = dict(getSigns())

#        sinal_erro.append(signs_dict["erro"])
#        sinal_diff.append(signs_dict["diff"])

    '''for data_of_interest in [sinal_diff, sinal_erro]:
        data_of_interest.append([signs_dict["diff"], signs_dict["erro"]'''

#    return times, sinal_diff, sinal_erro


start = time.time()
#times, sinal_diff, sinal_erro = update_signs_values(times, sinal_diff, sinal_erro)


# # # ESTRUTURA DASH # # #    

app.layout = html.Div([
                html.Div([
                       html.H3('Chiquinho Lifestyle',
                                style={'float': 'left', 'width': '100%'})  
                        ]),
                               
                       html.Div([
                            html.Div([
                                html.Div([
                                    dcc.Graph(id='diff-graph')
                                     ]
                                        ),
                                html.Div([
                                    dcc.Graph(id='err-graph')
                                     ]
                                        )
                                ], className = 'col s8 m8 l8'),
                            html.Div([
                                html.Div(
                                    dcc.Graph(id='bat-graph')
                                        )
                                 ], className = 'col s4 m4 l4'
                                        ) 
                            ], className = 'row'),

                       dcc.Interval(id='graph-signs-update',
                                    interval=1*1250, # intervalo de atualização dos sinais
                                    n_intervals=0
                                   ),

                       dcc.Interval(id='graph-bat-update',
                                    interval=5*1000, # intervalo de atualização da bateria
                                    n_intervals=0),

                       html.Div([
                           html.Div([
                                html.H5('Ordem'),

                                html.Label('V. Máxima'),
                                dcc.Input(id='vmax-input',
                                          value='', 
                                          type='number'),
                                html.Button('ENVIAR', 
                                            id = 'btn-set-vmax', 
                                            n_clicks=0),
                                html.Div(),
                                html.Button('S T A R T / S T O P', 
                                            id = 'btn-send-ordem',
                                            n_clicks=0),
                                    ], className = 'col center', 
                                       style = {'margin-left':2, 
                                                'margin-right':2,
                                                'width': '33%'}),                                          

                           html.Div([
                               html.H5('Constantes'),

                               html.Label('Kp'),
                               dcc.Input(id = 'kp-input', 
                                         value='', 
                                         type='number'),                   
                               html.Label('Ki'),
                               dcc.Input(id = 'ki-input', 
                                         value='', 
                                         type='number'),
                               html.Label('Kd'),
                               dcc.Input(id = 'kd-input', 
                                         value='', 
                                         type='number'),
                               html.Button('ATUALIZAR', 
                                           id='btn-get-cons', 
                                           n_clicks=0),
                               html.Div(id='hidden-div-send-cons', 
                                        style={'display': 'none'}),
                               html.Div(id='hidden-div-get-cons', 
                                        style={'display': 'none'}),
                               html.Div(id='hidden-div-set-vmax', 
                                        style={'display': 'none'}),
                               html.Div(id='hidden-div-connect', 
                                        style={'display': 'none'}),
                               html.Div(id='hidden-div-disconnect', 
                                        style={'display': 'none'}),
                               html.Button('ENVIAR', 
                                           id='btn-send-cons', 
                                           n_clicks=0)
                                    ], className = 'col center', 
                                       style={'margin-left':2, 
                                              'margin-right':2,
                                              'width': '33%'}),                                      
                                      
                           html.Div([
                               html.H5('Configurações'),

                               html.Label('IP: '),
                               html.Div([
                                   dcc.Input(id='ip-input', 
                                             value='',
                                             type="text")
                                ]),
                               html.Label('Rede: '),
                               html.Div([
                                    dcc.Input(id='ssid', 
                                              value='',
                                              type='text')
                                ]),
                               html.Label('BT: '),
                               html.Div([
                                   dcc.Input(id='end-bt', 
                                             value='',
                                             type='text')
                               ]),
                               html.Button('CONECTAR', 
                                           id = 'btn-conec', 
                                           n_clicks = 0),
                               html.Button('DISCONECTAR', 
                                           id='btn-disc',
                                           n_clicks=0)
                                    ], className = 'col center', #'col s4 m4 l4 center', 
                                       style={'margin-left':2,
                                              'margin-right':2,
                                              'width': '33%'})
                                                                                      
                            ], className='row') # style={'columnCount':3})

                      ], className="container", 
                         style={'width':'98%',
                                'margin-left':5,
                                'margin-right':5,
                                'max-width':50000}
                       )


# # # SIGN FIGURES CALLBACK # # #

@app.callback([Output('diff-graph', 'figure'), # Output('out-component', 'value')
               Output('err-graph', 'figure')], 

              [Input('graph-signs-update', 'n_intervals')] # Input('in-component', 'value')
             )
    
def update_signs_graphs(n):

    #update_signs_values(times, sinal_diff, sinal_erro)

    times.append(time.time() - start)

    if ws.connected:

        signs_dict = dict(getSigns())

        sinal_erro.append(signs_dict["erro"])
        sinal_diff.append(signs_dict["diff"])

    diff_figure = dict({"data": [{"x": list(times), 
                                  "y": list(sinal_diff), 
                                  "type": "scattergl"
                                  }],
                        "layout": {"title": {"text": "Sinal de Controle: u_diff"},
                                   "yaxis": {"range": [0, 4100]}}
                        })

    err_figure = dict({"data": [{"x": list(times), 
                                 "y": list(sinal_erro), 
                                 "type": "scattergl"
                                }],
                       "layout": {"yaxis": {"range": [0, 4100]},
                                  "title": {"text": "Sinal de Erro: erro"}}
                       })
                            

    return diff_figure, err_figure

# # # BAT FIGURE CALLBACK # # #

@app.callback(Output('bat-graph', 'figure'), # Output('out-component', 'value')

              [Input('graph-bat-update', 'n_intervals')] # Input('in-component', 'value')
             )

def update_bat_graph(n):

    if ws.connected:

        bat_dict = dict(getBat())
    

    bat_figure = go.Figure(data=go.Pie(labels=labels, values=values, hole=.4),
                           layout=go.Layout(title="Nível de bateria"))
                           
    
    '''bat_figure = dict({"data": {"values": [4095, bat_dict["bat"]],
                                 "labels": ['Total', 'Restante'],
                                 "type": "pie"
                                },
                       "layout": {"title": {"text": "Nível de Bateria"}}
                      })'''

    return bat_figure

# # # BUTTONS CALLBACK # # #

# # # botão "ATUALIZAR" Kp, Ki e Kd # # # 

@app.callback([Output('kp-input', 'value'),
               Output('ki-input', 'value'),
               Output('kd-input', 'value')
              ], # Output('out-component', 'value')

              [Input('btn-get-cons', 'n_clicks')]) # Input('in-component', 'value')

def btn_get_cons_callback(n):

    cons_dict = dict(getCons())
    
    cons_dict["kp"] = cons_dict["kp"]
    cons_dict["ki"] = cons_dict["ki"]
    cons_dict["kd"] = cons_dict["kd"]

    return cons_dict["kp"], cons_dict["ki"], cons_dict["kd"]

# # # botão "ENVIAR" Kp, Ki e Kd # # # 

@app.callback([Output('hidden-div-get-cons', 'figure')], # Output('out-component', 'value')

              [Input('btn-send-cons', 'n_clicks')],

              [State('kp-input', 'value'),
               State('ki-input', 'value'),
               State('kd-input', 'value')
              ]) # Input('in-component', 'value'))

def btn_send_cons_callback(n, kp_input, ki_input, kd_input):

    senCons(kp_input, ki_input, kd_input)

    return None

# # # botão "ENVIAR" Vmax # # # 

@app.callback([Output('hidden-div-send-cons', 'figure')], # Output('out-component', 'value')

              [Input('btn-set-vmax', 'n_clicks')],

              [State('vmax-input', 'value')] # Input('in-component', 'value')
             )

def btn_set_vmax_callback(n, vmax_value):

    senVmax(vmax_value)

    return None

# # # botão "START/STOP" # # # 

@app.callback([Output('hidden-div-set-vmax', 'figure')], # Output('out-component', 'value')

              [Input('btn-send-ordem', 'n_clicks')] # Input('in-component', 'value')
             )

def btn_send_ordem_callback(n):

    if andando:
        senOrd(ordem)
        ordem = "start"
        andando = False
    else:
        senOrd(ordem)
        ordem = "stop"
        andando = True

    return None

# # # botão "CONECTAR" # # # 

@app.callback([Output('end-bt', 'value'),
               Output('ssid', 'value')], # Output('out-component', 'value')
              
              [Input('btn-conec', 'n_clicks')], # Input('in-component', 'value')
              
              [State('ip-input', 'value')] 
             )

def btn_con_callback(n, ip_input):

    # ws.connect("ws://192.168.15.2:80")

    end_webs_comp = end_webs.format(ip_input)
    ws.connect(end_webs_comp)
    #ws.connect("ws://192.168.15.2:80")

    '''no python, não se pode armazenar um dicionário no outro, necessário
    transformar um dicionário em um dicionário e armazená-lo em um dicionário'''
    
    conf_dict = dict(getConf()) 

    '''ao invés de perguntar para a máquina o IP, basta apenas perguntar
    para o robô, pois os dois estarão na mesma rede'''

    return conf_dict["BT"], conf_dict["Rede"]

# # # botão "DISCONECTAR" # # # 

@app.callback(Output('hidden-div-disconnect', 'figure'), # Output('out-component', 'value')

              [Input('btn-disc', 'n_clicks')] # Input('in-component', 'value')
             )

def btn_disc_callback(n):
    
    ws.close()

    return None


if __name__ == '__main__':
    app.run_server(host='192.168.15.3', port=80) # host = 'IP local dá maquina' (checar antes de rodar)
    # app.run_server(debug=True)

'''

FIG AS DICT: min interval = 100 ms [abstração muito baixa, muito rápido e muito difícil de fazer manutenção]

    diff_figure = dict({"data": [{"x": list(times), 
                                  "y": list(sinal_diff), 
                                  "type": "scattergl"
                                  }],
                        "layout": {"title": {"text": "Sinal de Controle: diff"},
                                   "yaxis": {"range": [0, 100]}}
                                  
                        })

    err_figure = dict({"data": [{"x": list(times), 
                                 "y": list(sinal_erro), 
                                 "type": "scattergl"
                                }],
                       "layout": {"yaxis": {"range": [1000, 2000]},
                                  "title": {"text": "Sinal de Erro: err"}}
                                 
                       })

    bat_figure = dict({"data": [{"x": list(times),
                                 "y": list(bat_level),
                                  "type": "scatter"
                                }],
                       "layout": {"yaxis": {"range": [0, 10]},
                                  "title": {"text": "Nível de Bateria"}}
                                 

                       })

'''

'''

PLOTLY.GRAPH_OBJECTS: intervalo mínimo = 1000 ms [média abstração e não muito rápida]

    diff_figure = go.Figure(data = [go.Scatter(x = list(times), y = list(sinal_diff))],
                            layout = go.Layout(yaxis = dict(range = [-10,100]))
                            )

    err_figure = go.Figure(data = [go.Scatter(x = list(times), y = list(sinal_erro))],
                            layout = go.Layout(yaxis = dict(range = [500,2000]))
                           )

    bat_figure = go.Figure(data = [go.Scatter(x = list(times), y = list(bat_level))],
                            layout = go.Layout(yaxis = dict(range = [-1,10]))

'''

'''

PLOTLY.EXPRESS: intervalo mínimo = 1500 ms [alta abstração e por isso é lento]

diff_figure = px.line(x = times, y = sinal_diff, labels = {'x': 'Tempo (s)', 'y': 'Sinal de controle: d(V)'}, range_y=[-10,100])

err_figure = px.line(x = times, y = sinal_erro, labels = {'x': 'Tempo (s)', 'y': 'Sinal de Erro: e'}, range_y=[500,2000])

bat_figure = px.line(x = times, y = bat_level, labels = {'x': 'Tempo (s)', 'y': 'Nível de bateria: '}, range_y=[-1, 9])

'''