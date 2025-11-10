def message_route(message):
    if '"action":"player/register"' in message:
        return
    if '"action":"player/login"' in message:
        return
    if '"action":"themes/list"' in message:
        return
    if '"action":"sessions/list"' in message:
        return
    if '"action":"session/create"' in message:
        return
    if '"action":"session/join"' in message:
        return
    if 'POST session/started' in message:
        return
    if 'POST question/new' in message:
        return
    if '"action":"joker/use"' in message:
        return
    if '"action":"question/answer"' in message:
        return
    if 'POST question/results' in message:
        return
    if 'POST session/player/eliminated' in message:
        return
    if 'POST session/finished' in message:
        return
    if 'POST session/player/left' in message:
        return
    else:
        "MSG UNKNOWN"