//
//  SwarmAi.AntMiner
//  Test2
//
//  Created by Ярослав Карпунькин on 07.10.2020.
//

import UIKit

enum Constants {
    static let antsCount = 10
    static let basePheromonLevel: Double = 1
    static let startFeatureName = "StartFeature"
    static let startCategoryName = "StartCategory"
    static let average = 2 // среднее число категорий на одну фичу
    static let eps: Double = 0.0005
    
    // Формула подсчета вероятности перехода
    static let alphaPower: Double = 1
    static let bettaPower: Double = 2
    
    // Формула испарения феромона
    static let p = 0.9 // p - коэффициент испарения
    
    // Формула остановки алгоритма
    static let pBest = 0.1
}

// Класс переменной
class Feature {
    let name: String
    var categories: [Category]
    
    init(name: String, categories: [Category]) {
        self.name = name
        self.categories = categories
    }
    
    class Category: Hashable {
        static func == (lhs: Feature.Category, rhs: Feature.Category) -> Bool {
            return lhs.name == rhs.name
        }
        
        public func hash(into hasher: inout Hasher) {
             hasher.combine(name)
        }
        
        let name: String
        
        init(name: String) {
            self.name = name
        }
    }
}

typealias Category = Feature.Category

class Rule {
    var catygories: [Category] = []
    
    func add(category: Category) {
        catygories.append(category)
    }
}

class Edge {
    let from: Category
    let to: Category
    
    var pheromoneLevel: Double
    var probability: Double
    
    init(from: Category, to: Category, pheromoneLevel: Double, probability: Double) {
        self.from = from
        self.to = to
        self.probability = probability
        self.pheromoneLevel = pheromoneLevel
    }
    
    func describe() -> String {
        return "Edge: \(from.name) -> \(to.name), prob:\(probability) pLevel:\(pheromoneLevel) "
    }
}

class DataSetLine {
    var lines: [Category]
    let classificationValue: Bool
    
    init(lines: [Category], classificationValue: Bool) {
        self.lines = lines
        self.classificationValue = classificationValue
    }
    
    static func buildFromStrings(stringCategory: [String], classificationValue: Bool) -> DataSetLine {
        return DataSetLine.init(lines: stringCategory.map{.init(name: $0)}, classificationValue: classificationValue)
    }
}

class AntMiner {
    // default
    private var data: [DataSetLine] = AntMiner.getDefaultDataSet() // датасет
    private var featuresNames: [String] = AntMiner.getDefaultFeatures() // массив с названиями переменных
    private lazy var startCategory: Category = .init(name: Constants.startCategoryName) // Точка старта
    private lazy var startFeature: Feature = .init(name: Constants.startFeatureName, categories: [startCategory])
    
    // main
    private var features: [Feature] = [] // массив с переменными
    private var graph: [Category : Array<Edge>] = [:] // главный граф
    private var heuristics: [Category : Double] = [:] // значения эвристиков в категориях на базе датасета
    
    // find rule
    var bestConfidence: Double = 0 // оптимальная достоверность правила
    var bestRule = Rule() // оптимальное правило
    var bestRuleEdges: [Edge] = [] // ребра оптимального правила
    var globalSuccessCoverage: Double = 0 // покрытие правила
    var globalUncoverage: Double = 0 // число невычеркнутых строк в датасете
    var antsIterationNumber: Int = 0 // количество раундов запуска муравьев
    
    init() {}
    
    /// Найти правило
    func findRule() -> Rule {
        buildFeatures()
        buildGraph()
        calculateHeuristics()
        calculateProbabilities()
        debugPrintGraphDescription()

        while true {
            let rule = runAnts()
            antsIterationNumber += 1
            if let rule = rule {
                return rule
            }
        }
    }
    
    /// Вывести в консоль текущее состояние графа
    func debugPrintGraphDescription() {
        graph.forEach{ val in
            print(val.value.map{$0.describe()})
        }
    }

    // запускаем прохождение по графу
    private func runAnts() -> Rule? {
        for _ in 0..<Constants.antsCount {
            print("-------")
            var currentNode = startCategory
            var ruleEdges: [Edge] = []
            let currentRule = Rule()
            
            // прохождение по графу
            while currentRule.catygories.count != features.count - 1 {

                if currentNode != startCategory {
                    currentRule.add(category: currentNode)
                }
                // выходим в случае, если количество категорий в правиле равно количеству переменных в датасете
                if currentRule.catygories.count == features.count - 1  {
                    break
                }
                
                let edges = graph[currentNode]!
                let targetEdge = findEdge(edges: edges)
                currentNode = targetEdge.to
                ruleEdges.append(targetEdge)
            }
            
            print("Current rule edges:")
            ruleEdges.forEach { edge in
                print(edge.describe())
            }
            
            let successCoverage = data.filter { line in
                currentRule.catygories.map{$0.name} == line.lines.map{$0.name} && line.classificationValue
            }.count // процент строк, которые дают значение true для данного правила
            
            let totalCoverage = data.filter { line in
                currentRule.catygories.map{$0.name} == line.lines.map{$0.name}
            }.count // процент строк, которые удовлетворяют данному правилу
            
            let uncoverage: Double = Double(data.count)
            let confidence: Double = Double(successCoverage / totalCoverage)  // достоверность
            print("confidence\(confidence), best confidence \(bestConfidence)")
            
            if confidence > bestConfidence {
                bestConfidence = confidence
                bestRule = currentRule
                globalSuccessCoverage = Double(successCoverage)
                globalUncoverage = uncoverage
                bestRuleEdges = [] + ruleEdges
            }
            print("Current rule: \(currentRule.catygories.map{$0.name})")
            print("Best rule: \(bestRule.catygories.map{$0.name})")
            print("----")
        }

        
        evaporatePheromone() // испаряем феромон
        // Значение следа, которое будем добавлять на успешные ребра
        let deltaBest: Double = Double(bestConfidence) + Double(globalSuccessCoverage) / Double(globalUncoverage)
        print("Delta best: \(deltaBest)")
        if deltaBest == 0 {
            fatalError("Delta best is zero")
        }
        
        bestRuleEdges.forEach { edge in
            edge.pheromoneLevel = edge.pheromoneLevel + deltaBest
        }
        print("Edges after evaporate and pheromone increace:")
        bestRuleEdges.forEach { edge in
            print(edge.describe())
        }
        print("\n")
        
        let tMax = (1 / (1 - Constants.p)) * deltaBest // Нижняя граница для ребер, входящих в оптимальный путь
        let tMin = (tMax * (1 - pow(Constants.pBest, Double(1) / Double(featuresNames.count)))) / ((Double(Constants.average - 1)) * pow(Constants.pBest, Double(1) / Double(featuresNames.count))) //  Верхняя граница для ребер, не входящих в оптимальный путь
        
        let successToTMax = bestRuleEdges.filter{
            $0.pheromoneLevel >= tMax - Constants.eps
        }.count == featuresNames.count
        
        let successToTmin = Array(graph.values).flatMap {$0}.filter {
            $0.pheromoneLevel > tMin
        }.count == featuresNames.count
        
        
        if successToTMax && successToTmin {
            print("Found success rule!!!!!!")
            return bestRule
        }
        
        print("Tmax: \(tMax)")
        print("Tmin: \(tMin)")
        print("Go to next run\n")
        return nil
    }
    
    
    // испарить феромон
    private func evaporatePheromone() {
        for edges in Array(graph.values) {
            for currentEdge in edges {
                currentEdge.pheromoneLevel = Constants.p * currentEdge.pheromoneLevel
            }
        }
    }
    
    // выбираем на базе рандома и вероятностей ребро
    private func findEdge(edges: Array<Edge>) -> Edge {
        let rand = Double(CGFloat.random(in: 0..<1))
        var counter: Double = 0
        
        for edge in edges {
            counter += edge.probability
            if rand < counter {
                return edge
            }
        }
        return edges[edges.count - 1]
    }
    
    // инициализируем массив с фичами
    private func buildFeatures() {
        var categories: [Set<Category>] = [Set<Category>].init(repeating: Set<Category>(), count: featuresNames.count)
        for datum in data {
            for i in 0..<datum.lines.count {
                categories[i].insert(datum.lines[i])
            }
        }
        // нулевая фича
        features = [startFeature] + categories
            .map{Array<Category>($0)}
            .enumerated()
            .map({ (index, element) in
                .init(name: featuresNames[index], categories: element)
            })
    }
    
    // создаем граф с ребрами
    private func buildGraph() {
        for i in 1..<features.count {
            for currentCategory in features[i-1].categories {
                var tempEdges = [Edge]()
                for nextCategory in features[i].categories {
                    tempEdges.append(Edge.init(from: currentCategory , to: nextCategory, pheromoneLevel: Constants.basePheromonLevel, probability: 0))
                }
                graph[currentCategory] = tempEdges
            }
        }

    }
    
    // считаем эвристические значения на базе датасета
    private func calculateHeuristics() {
        for feature in features {
            guard feature.name != Constants.startFeatureName else {
                continue
            }
            for categorie in feature.categories {
                
                let successСount = data.filter{
                    $0.lines.contains(categorie) && $0.classificationValue
                }.count
                let generalCount = data.filter {
                    $0.lines.contains(categorie)
                }.count
                
                heuristics[categorie] = Double(successСount) / Double(generalCount)
            }
        }
        heuristics.forEach { (key: Category, value: Double) in
            print("\(key.name) \(value)")
        }
    }
    
    // считаем вероятности захода в ноду
    func calculateProbabilities() {
        for edges in Array(graph.values) {
            for currentEdge in edges {
                let pNumerator = pow(currentEdge.pheromoneLevel, Constants.alphaPower) * pow(heuristics[currentEdge.to]!, Constants.bettaPower)
                
                var pDenominator: Double = 0
                for edge in edges {
                    pDenominator += pow(edge.pheromoneLevel, Constants.alphaPower) * pow(heuristics[edge.to]!, Constants.bettaPower)
                }
                
                currentEdge.probability = pNumerator/pDenominator
            }
        }
    }
}


extension AntMiner {
    static func getDefaultDataSet() -> [DataSetLine] {
        return [
            .buildFromStrings(stringCategory: ["YELLOW", "SMALL", "STRETCH", "ADULT"], classificationValue: true),
            .buildFromStrings(stringCategory: ["YELLOW", "SMALL", "STRETCH", "CHILD"], classificationValue: true),
            .buildFromStrings(stringCategory: ["YELLOW", "SMALL", "DIP", "ADULT"], classificationValue: true),
            .buildFromStrings(stringCategory: ["YELLOW", "SMALL", "DIP", "CHILD"], classificationValue: true),
            
            .buildFromStrings(stringCategory: ["YELLOW", "LARGE", "STRETCH", "ADULT"], classificationValue: true),
            .buildFromStrings(stringCategory: ["YELLOW", "LARGE", "STRETCH", "CHILD"], classificationValue: false),
            .buildFromStrings(stringCategory: ["YELLOW", "LARGE", "DIP", "ADULT"], classificationValue: false),
            .buildFromStrings(stringCategory: ["YELLOW", "LARGE", "DIP", "CHILD"], classificationValue: false),
            
            .buildFromStrings(stringCategory: ["PURPLE", "SMALL", "STRETCH", "ADULT"], classificationValue: true),
            .buildFromStrings(stringCategory: ["PURPLE", "SMALL", "STRETCH", "CHILD"], classificationValue: false),
            .buildFromStrings(stringCategory: ["PURPLE", "SMALL", "DIP", "ADULT"], classificationValue: false),
            .buildFromStrings(stringCategory: ["PURPLE", "SMALL", "DIP", "CHILD"], classificationValue: false),
            
            .buildFromStrings(stringCategory: ["PURPLE", "LARGE", "STRETCH", "ADULT"], classificationValue: true),
            .buildFromStrings(stringCategory: ["PURPLE", "LARGE", "STRETCH", "CHILD"], classificationValue: false),
            .buildFromStrings(stringCategory: ["PURPLE", "LARGE", "DIP", "ADULT"], classificationValue: false),
            .buildFromStrings(stringCategory: ["PURPLE", "LARGE", "DIP", "CHILD"], classificationValue: false),
        ]
    }
    
    static func getDefaultFeatures() -> [String] {
        return ["color", "size", "act", "age"]
    }
}


let antminer = AntMiner.init()
print("Final rule \(antminer.findRule().catygories.map{$0.name}), round:\(antminer.antsIterationNumber)")
