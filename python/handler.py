import json

CLICK_TYPE_MESSAGE = {
  'SINGLE': 'ON  MEETING',
  'DOUBLE': 'OFF MEETING',
  'LONG':'END OF WORK'
}
CLICK_TYPE_COLOR = {
  'SINGLE': '0xF800',
  'DOUBLE': '0x000F',
  'LONG': '0xFD20'
}
CLICK_TYPE_INT = {
  'SINGLE': 1,
  'DOUBLE': 2,
  'LONG':3
}
import boto3
iot = boto3.client('iot-data')
def lambda_handler(event, context):
    print(json.dumps(event, ensure_ascii=False))
    if 'deviceEvent' in event:
        click_type = event['deviceEvent']['buttonClicked']['clickType']  # LTE-M Button powered by AWS
    else:
        click_type = event['clickTypeName'] # LTE-M Button for Enterprise/LTE-M Button Plus

    try:
        message = CLICK_TYPE_MESSAGE[click_type]
        color = CLICK_TYPE_COLOR[click_type]
        status = CLICK_TYPE_INT[click_type]
    except KeyError:
        return
    body = {
        'message': message,
        'color': color,
        'status':status
    }
    payload = json.dumps(body, ensure_ascii=False)
    iot.publish(topic='iotbutton/status', payload=payload, qos=0)
    response = {
        'statusCode': 202,
        'body': json.dumps(event, ensure_ascii=False)
    }
    return response
