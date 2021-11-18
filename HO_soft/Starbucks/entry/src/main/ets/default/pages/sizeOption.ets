class Cup {
  name: string
  image: string

  constructor(name, image) {
    this.name = name
    this.image = image
  }
}

@Entry
@Component
export default struct SizeOption {
  @State private cups: Cup[] = [
    new Cup($r("app.string.tall_cup"), $r("app.media.tall_cup")),
    new Cup($r("app.string.grande_cup"), $r("app.media.grande_cup")),
    new Cup($r("app.string.venti_cup"), $r("app.media.venti_cup")),
  ]

  @State private cupsIndices : number[] = this.cups.map((_, index) => index)
  @State selection: number = 0

  build() {
    Flex({ justifyContent: FlexAlign.SpaceAround }) {

      ForEach(this.cupsIndices, (index) => {
        Column() {
          Image(this.cups[index].image)
            .width(63)
            .height(138)
            .objectFit(ImageFit.Contain)
            .renderMode(this.selection == index ? ImageRenderMode.Original : ImageRenderMode.Template)
          Text(this.cups[index].name)
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
            .fontColor(this.selection == index ? $r("app.color.theme") : $r("app.color.unselect"))
        }
        .padding(10)
        .onClick(() => {
          this.selection = index
        })
      })

      Text(this.selection.toString()).fontSize(30)
      .fontColor($r("sys.color.id_color_foreground"))
    }
    .width('100%')
    .borderWidth(1)
    .borderColor($r("app.color.theme"))
    .borderRadius(18)
  }
}