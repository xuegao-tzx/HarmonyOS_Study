import SizeOption from './sizeOption.ets'

@Entry
@Component
struct Index {
  build() {
    Flex({
      direction: FlexDirection.Column,
      alignItems: ItemAlign.Center,
      justifyContent: FlexAlign.Center
    }) {
      SizeOption({selection: 1})
    }
    .width('100%')
    .height('100%')
    .padding(20)
  }
}